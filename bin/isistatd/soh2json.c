/*======================================================================
 *
 *  HTTP Post IDA NRTS SOH info to IDA STATUS WEB SERVICE API
 *
 *====================================================================*/
#include "soh2json.h"

extern char *VersionIdentString;

static time_t now;
static struct {
    struct tm tm;
    char buf[64];
} gm, local;

static BOOL debug = FALSE;

#define LIVE_LATENCY_THRESHOLD 21600  // 6 hrs


json_t *ChannelStatus(char *sta, char *chn, char *loc, REAL64 srate, UINT32 nseg, REAL64 tols, REAL64 tslw)
{
UINT32 latency;
char tbuf[1024];
time_t atime_t;
struct tm atime_tm;

json_t *j_cnf_obj;
// json_error_t *j_err;

    // initializer cnf json obj
    j_cnf_obj = json_object();

    time_t now = time(NULL);
    latency = now - (UINT32) tols;

    json_object_set_new(j_cnf_obj, "chn", json_string(chn));
    json_object_set_new(j_cnf_obj, "loc", json_string(loc));

    // use UTC time and format for PostGreSQL "timestamp without a timezone (treated as UTC)"
    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S -00:00", gmtime_r(&now, &atime_tm));
    json_object_set_new(j_cnf_obj, "reported_at", json_string(tbuf));

    json_object_set_new(j_cnf_obj, "freq", json_real(srate));
    json_object_set_new(j_cnf_obj, "nseg", json_integer(nseg));
    
    if (tols != (REAL64) ISI_UNDEFINED_TIMESTAMP) {

        // these only exist when for channels with actual telemtry data

        // use UTC time and format for PostGreSQL "timestamp without a timezone (treated as UTC)"
        atime_t = (const time_t)tols;
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S -00:00", gmtime_r(&atime_t, &atime_tm));
        json_object_set_new(j_cnf_obj, "chn_most_recent_data",      json_string(tbuf));
        json_object_set_new(j_cnf_obj, "chn_data_latency_secs",   json_integer((INT32)latency));

        // use UTC time and format for PostGreSQL "timestamp without a timezone (treated as UTC)"
        atime_t = (const time_t)(now - tslw);
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S -00:00", gmtime_r(&atime_t, &atime_tm));
        json_object_set_new(j_cnf_obj, "chn_last_write_at",      json_string(tbuf));
        json_object_set_new(j_cnf_obj, "chn_write_latency_secs",   json_integer((INT32)tslw));

    }

    return j_cnf_obj;

}

static json_t *StationStatus(char *sta, REAL64 tols, REAL64 tslw, UINT32 livechn, UINT32 nseg)
{
UINT32 data_latency;
char tbuf[1024];
time_t atime_t;
struct tm atime_tm;

    json_t *j_res = json_object(); 

    time_t now = time(NULL);
    data_latency = time(NULL) - (UINT32) tols;

    // set fields into JSON structs...
    json_object_set_new(j_res, "sta", json_string(sta));

    // use UTC time and format for PostGreSQL "timestamp without a timezone (treated as UTC)"
    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S -00:00", gmtime_r(&now, &atime_tm));
    json_object_set_new(j_res, "reported_at", json_string(tbuf));

    json_object_set_new(j_res, "livechn_pcnt", json_integer(livechn));

    if (tols != (REAL64) ISI_UNDEFINED_TIMESTAMP) {

        // these only exist when the station has one or more channels with actual telemtry data

        json_object_set_new(j_res, "nseg_avg", json_integer(nseg));

        // use UTC time and format for PostGreSQL "timestamp without a timezone (treated as UTC)"
        atime_t = (const time_t)tols;
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S -00:00", gmtime_r(&atime_t, &atime_tm));
        json_object_set_new(j_res, "sta_most_recent_data", json_string(tbuf));
        json_object_set_new(j_res, "sta_data_latency_secs",   json_integer((INT32)data_latency));

        // use UTC time and format for PostGreSQL "timestamp without a timezone (treated as UTC)"
        atime_t = (const time_t)(now - tslw);
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S -00:00", gmtime_r(&atime_t, &atime_tm));
        json_object_set_new(j_res, "sta_last_write_at",      json_string(tbuf));
        json_object_set_new(j_res, "sta_write_latency_secs",   json_integer((INT32)tslw));

    }

    return j_res;
}

void ProcessIsiInfo(char *server, char *apiurl, ISI_SOH_REPORT *soh, ISI_CNF_REPORT *cnf)
{
/*
    Loop through Soh and Cnf contruct "soh" status for each station as JSON
    Each station posted with separate HTTP call
*/
CURL *curl;
struct curl_slist *list = NULL;
static char errbuf[CURL_ERROR_SIZE];
CURLcode res;

// summary vars for each sta
int livechn; // percentage of "live" CHNs for STA
int totchn; // total # of CHN's for STA

int sumnseg,totnseg, nsegval;

REAL64 sumtols;
REAL64 sumtslw;
UINT32 nseg;
// ISI_STREAM_CNF *cnfList[soh->nentry];

// loop control vars
UINT32 ndx = 0;
char *cur_sta, *new_sta;
static char *Blank = "    ";

// json related vars
json_t *j_root;
json_t *j_stat_res;
json_t *j_chn_arr;


    curl_global_init(CURL_GLOBAL_ALL);
 
    /* get a curl handle */ 
    curl = curl_easy_init();

    if ((curl) && (soh->nentry > 0)) {

        curl_easy_setopt(curl, CURLOPT_URL, apiurl);

        // curl_easy_setopt(curl, CURLOPT_PORT, 3001);

        // set HTTP headers
        list = curl_slist_append(list, "Accept:");
        list = curl_slist_append(list, "Content-Type: application/json");
        list = curl_slist_append(list, "Accept-Language: en-us");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        /* provide a buffer to store errors in */
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

        /* set SSL protocol version TLS 1.2 */
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);


        // initializer root json obj
        j_root = json_object();
        // // create chn array for sta channels
        // j_chn_arr = json_array();

        // initialize everything
        cur_sta = soh->entry[ndx].name.sta;
        new_sta = soh->entry[ndx].name.sta;
        ndx = 0;

        sumtols = (REAL64) ISI_UNDEFINED_TIMESTAMP;
        sumtslw = (REAL64) ISI_UNDEFINED_TIMESTAMP; // overall station time since last write (tslw)
        nseg = 0;
        livechn = 0;
        totchn = 0;
        totnseg = 0;
        sumnseg = 0;

        // loop through all soh entries...
        do {

            // create chn array for sta channels
            j_chn_arr = json_array();


            // loop through chn records for sta summary stats
            do {

                if (debug) { 
                    fprintf(stdout, "Processing (sta chn loc tslw) : %s %s %s %10.2f\n", soh->entry[ndx].name.sta, soh->entry[ndx].name.chn, soh->entry[ndx].name.loc, soh->entry[ndx].tslw); 
                }

                if (soh->entry[ndx].tols.value != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
                    if (sumtols == (REAL64) ISI_UNDEFINED_TIMESTAMP || soh->entry[ndx].tols.value > sumtols) {
                        sumtols = soh->entry[ndx].tols.value;
                    }
                }
                nseg = soh->entry[ndx].nseg;
                sumnseg = sumnseg + nseg;

                if (soh->entry[ndx].tslw != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
                    if (sumtslw == (REAL64) ISI_UNDEFINED_TIMESTAMP || soh->entry[ndx].tslw < sumtslw) {
                        sumtslw = soh->entry[ndx].tslw;
                    }
                }

                // only count Continuous channels for live_pcnt
                // skip chn beginning with 'e' until DB can be used directly
                if ( strncmp( &cnf->entry[ndx].name.chn, "e", 1) != 0) {

                    if ((soh->entry[ndx].nrec != 0) && 
                        ((UINT32) soh->entry[ndx].tslw <= LIVE_LATENCY_THRESHOLD) &&
                        (soh->entry[ndx].tslw != (REAL64) ISI_UNDEFINED_TIMESTAMP)) {
                        livechn++; // if channel is reporting, count it
                    }
                    // count total channels for overall station percentage calculation 
                    totchn++;    
                }

                // count total nseg for chn if value > 0, used to calc avg nseg for only chn's with nseg > 0
                // totnseg is the number of channels for this sta with > 0 nsegs
                if (nseg > 0) totnseg++;

                REAL64 ssrate = 1.0 / isiSrateToSint(&cnf->entry[ndx].srate);

                int err = json_array_append_new(j_chn_arr, ChannelStatus(cur_sta, soh->entry[ndx].name.chn, soh->entry[ndx].name.loc, ssrate, soh->entry[ndx].nseg, soh->entry[ndx].tols.value, soh->entry[ndx].tslw));
                if (err != 0) { fprintf(stderr, "Error construcrting channel satus array in StationStatus: %d", err); }

                // if (debug) { fprintf(stdout, "Will append channel info for station: %s\n", cur_sta); }

                // increm,ent loop ndx and 
                // get sta for next chn, or set to blank if at end of list
                ndx++;
                new_sta = (ndx < soh->nentry) ? soh->entry[ndx].name.sta : Blank;

            } while (strcmp(cur_sta, new_sta) == 0);

            // at end of cur_sta channels...

            // compute summary stats for cur_sta
            if (totchn > 0) { livechn = (livechn * 100) / totchn; } else { livechn = 0; }
            if (totnseg > 0) { nsegval = sumnseg / totnseg; } else { nsegval = 0; }


            // construct JSON 
            // get station level results
            j_stat_res = StationStatus(cur_sta, sumtols, sumtslw, livechn, nsegval);
            // append chhannel info...
            // json_object_set_new(j_stat_res, "chns_attributes", j_chn_arr);
            json_object_set_new(j_stat_res, "chns", j_chn_arr);
            // free(j_chn_arr);

            // set station status object into root
            json_object_set_new(j_root, "soh", j_stat_res);
            json_object_set_new(j_root, "hostname", json_string(server));

            // if (debug) { fprintf(stdout, "Will create JSON for station: %s\n", cur_sta); }

            // dump json_t root object to (char *)
            char *json = json_dumps(j_root, JSON_REAL_PRECISION(2) | JSON_ENSURE_ASCII | JSON_INDENT(4) | JSON_PRESERVE_ORDER);

            // Send for Posting...
            if (debug) { fprintf(stdout, "Will POST JSON for station: %s to %s\n", cur_sta, apiurl); }

            // Now specify the actual JSON payload as POST data  
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
            res = curl_easy_perform(curl);
            /* Check for errors */ 
            if(res != CURLE_OK) {
                fprintf(stderr, "\n\ncurl_easy_perform() failed: %s\n\n", errbuf);
                exit(1);
            }

            if (debug) { fprintf(stdout, "Did POST JSON for station: %s to %s\n", cur_sta, apiurl); }

            // cleanup...
            free(json);

            // clear root 'soh' objects and channel array
            json_array_clear(j_chn_arr);
            json_object_clear(j_stat_res);
            json_object_clear(j_root);
            // free j_stat_res created by calling function StationStatus

            // free(j_stat_res);
            // free(j_chn_arr);

            // re-=initialize for next sta
            sumtols = (REAL64) ISI_UNDEFINED_TIMESTAMP;
            sumtslw = (REAL64) ISI_UNDEFINED_TIMESTAMP;
            nseg = 0;
            livechn = 0;
            totchn = 0;
            totnseg = 0;
            sumnseg = 0;

            // set cur_stat to new_sta for next sta
            cur_sta = new_sta;

        } while (ndx < soh->nentry);

        // at end of soh list... our work hrer is done
        // free json mem
        // free(j_chn_arr);
        json_decref(j_root);

        // clean up curl
        curl_slist_free_all(list); /* free the header list */
        curl_easy_cleanup(curl);
        curl_global_cleanup();

    }
}
/*-----------------------------------------------------------------------+
 |                                                                       |       
 | Copyright (C) 2015 Regents of the University of California            |       
 |                                                                       |       
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |       
 | damages arising from the use of this software.                        |       
 |                                                                       |       
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |       
 |                                                                       |       
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |       
 |    documentation of the contribution by Project IDA, UCSD would be    | 
 |    appreciated but is not required.                                   |       
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |       
 | 3. This notice may not be removed or altered from any source          |       
 |    distribution.                                                      |       
 |                                                                       |       
 +-----------------------------------------------------------------------*/

