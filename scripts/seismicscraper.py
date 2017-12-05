#!/usr/bin/env python3.4
# duaerbach dev system is plain python

import copy
import argparse
import re
import datetime as dt
import calendar as cal
import requests
from bs4 import BeautifulSoup
import matplotlib.pyplot as plt


parser = argparse.ArgumentParser(description="IDA query tool for IRIS SeismiQuery")
parser.add_argument('year', type=int, help="YEAR: (4-digits) to query", choices=range(2000, dt.date.today().year + 1))
parser.add_argument('month', type=int, help="MONTH: to query", choices=range(1,13))
parser.add_argument("station", help="STATION code to check")
parser.add_argument("-n", "--network", dest='network', default="II", help="Newtwork code")
parser.add_argument("-v", "--verbose", type=int, dest='verbosity', default=1, help="Set Verbose level", choices=[0,1,2])

args = parser.parse_args()

URL_ROOT = 'http://ds.iris.edu/SeismiQuery/bin/caltimeseries2.pl?'
NET = args.network
STA = args.station
YEAR = args.year
MONTH = args.month
_, month_len = cal.monthrange(YEAR,MONTH)
JDAY_START = int(dt.date(YEAR, MONTH, 1).strftime("%j"))


HTML_TR_NET_NDX      = 0
HTML_TR_STATION_NDX  = 1
HTML_TR_CHANNEL_NDX  = 2
HTML_TR_STARTTIME_NDX = 3
HTML_TR_ENDTIME_NDX  = 4
HTML_TR_LOC_NDX      = 5

RESREC_CHANNEL_NDX  = 2
RESREC_LOC_NDX  = 3
RESREC_JDAYSTR_NDX = 5
RESREC_DAY_SECS_NDX = 7
RESREC_DAY_PCNT_NDX = 8
RESREC_CHANKEY_NDX = 9

SECONDS_PER_DAY = 86400

COMBINED_CHANNEL_LOC_CODE = 'CC'


def process_channel_group(chan_interval_list, year_str, jday_str, verbosity=0):

    # assumed chan_interval_list will have 1+ rows

    # trim start and end to be within the current JDAY 
    jday_start = dt.datetime.strptime(year_str+'-'+jday_str, '%Y-%j')
    jday_end = jday_start + dt.timedelta(days=1)

    # trim all intervals to current jDAY
    for row in chan_interval_list:
        start_dt = max(jday_start, dt.datetime.strptime(row[HTML_TR_STARTTIME_NDX],  '%Y-%m-%d %H:%M:%S'))
        end_dt   = min(jday_end, dt.datetime.strptime(row[HTML_TR_ENDTIME_NDX],  '%Y-%m-%d %H:%M:%S'))
        row[HTML_TR_STARTTIME_NDX] = start_dt.isoformat()
        row[HTML_TR_ENDTIME_NDX] = end_dt.isoformat()
        if verbosity>=2:
            print(row)

    # sort by start, end timestamps
    chan_interval_list.sort(key=lambda row: row[HTML_TR_STARTTIME_NDX] + row[HTML_TR_ENDTIME_NDX])

    total_seconds = 0
    percentage = 0.0

    intervals = []

    for row in chan_interval_list:
        start_dt = dt.datetime.strptime(row[HTML_TR_STARTTIME_NDX],  '%Y-%m-%dT%H:%M:%S')
        end_dt = dt.datetime.strptime(row[HTML_TR_ENDTIME_NDX],  '%Y-%m-%dT%H:%M:%S')
        if len(intervals) == 0:
            intervals.append([start_dt, end_dt])
        else:
            prev = intervals[-1]
            prev_start_dt = prev[0]
            prev_end_dt = prev[1]
            if start_dt <= prev_end_dt:
                intervals[-1] = [prev[0], end_dt]
            else:
                intervals.append([start_dt, end_dt])


    for ntr in intervals:
        if verbosity>=2:
            print(ntr)
        total_seconds += int((ntr[1] - ntr[0]).total_seconds())

    percentage = round(100.0*total_seconds/SECONDS_PER_DAY, 1)

    resdict = {
        'NET'         : arow[HTML_TR_NET_NDX],
        'STA'         : arow[HTML_TR_STATION_NDX],
        'CHAN'        : arow[HTML_TR_CHANNEL_NDX],
        'LOC'         : arow[HTML_TR_LOC_NDX],
        'DATA_SECS'   : total_seconds,
        'DATA_PCNT'   : percentage
    }

    return resdict


fname = NET+'-'+STA+'-'+str(YEAR)+'-'+str(MONTH).zfill(2)+'_SQRES'
fl = open(fname+'.csv', 'w')
fl.write(', '.join(['NET', 
    'STA', 
    'CHAN', 
    'LOC', 
    'YEAR', 
    'JDAY', 
    'ISODATE', 
    'DATA_SECS', 
    'DATA_PCNT', 
    'CHAN_KEY',
    'QUERY_TS']) + '\n')

results = []
data_channels = {}
plotdata = {}
monthly_chankey_list = []
monsum = {} # for monthly summaries

for JDAY in range(JDAY_START,JDAY_START + month_len):
    JDAYSTR = str(JDAY).zfill(3)

    if args.verbosity>0:
        print('Sending query for:', NET, STA, YEAR, JDAYSTR, '...')

    res = requests.get(URL_ROOT + NET+'+'+STA+'+'+str(YEAR)+'+'+JDAYSTR+'+1')

    query_timestamp = dt.datetime.now(dt.timezone.utc)

    htmldata = res.text
    htmlsoup = BeautifulSoup(htmldata, "lxml")
    # datatrs = list(htmlsoup.table.find_all('tr'))[1:] # skip header record
    BH_trs = [chanrow.parent for chanrow in htmlsoup.find_all('td', string=re.compile('^\nBH[12NEZ]$'))]

    if len(BH_trs) == 0:
        if args.verbosity>0:
            print('No data found for:', NET, STA, YEAR, JDAYSTR)
        continue
    else:
        if args.verbosity>0:
            print('   processing',len(BH_trs), 'records.')
    # pull data out of HTML structure (skip last, empty, HTML column)
    data_rows = [[cell.text.strip() for cell in tr.find_all('td')][:-1] for tr in BH_trs]

    # sort on chankey, plus start, end times. 
    # including times in sort to help with time interval merging
    data_rows.sort(key=lambda row: row[HTML_TR_NET_NDX] + '-' + \
                row[HTML_TR_STATION_NDX] + '-' + \
                row[HTML_TR_CHANNEL_NDX] + '-' + \
                row[HTML_TR_LOC_NDX] + '-' + \
                row[HTML_TR_STARTTIME_NDX] + '-' + \
                row[HTML_TR_ENDTIME_NDX])

    data_count = len(data_rows)

    # loop through row list nad package into chankey groups
    for row in data_rows:
        # first deal with specific channel
        chankey = row[HTML_TR_NET_NDX] + '-' + \
                row[HTML_TR_STATION_NDX] + '-' + \
                row[HTML_TR_CHANNEL_NDX] + '-' + \
                row[HTML_TR_LOC_NDX]
        if chankey not in data_channels.keys():
            data_channels[chankey] = []
        data_channels[chankey].append(row)
        if chankey not in monthly_chankey_list:
            monthly_chankey_list.append(chankey)

        # now leave off LOC code to get STA level percentages
        chankey = row[HTML_TR_NET_NDX] + '-' + \
                row[HTML_TR_STATION_NDX] + '-' + \
                row[HTML_TR_CHANNEL_NDX] + '-' + COMBINED_CHANNEL_LOC_CODE # use 'AA' just to distinguish the STA level resutls

        if chankey not in data_channels.keys():
            data_channels[chankey] = []
        cc_row = copy.copy(row)
        cc_row[HTML_TR_LOC_NDX] = COMBINED_CHANNEL_LOC_CODE
        data_channels[chankey].append(cc_row) # need to DUPE row to treat as separate data
        if chankey not in monthly_chankey_list:
            monthly_chankey_list.append(chankey)

#  just for checking/debugging
    group_count = 0
    for chankey, chandata in data_channels.items():
        if len(chandata) > 0:
            arow = chandata[0]
            if args.verbosity>=2:
                print(chankey)
            chan_result_dict = process_channel_group(chandata, str(YEAR), JDAYSTR, args.verbosity)
            group_count += len(chandata)
            # print(chankey)
            # print(chandata)
            results.append([
                chan_result_dict['NET'],
                chan_result_dict['STA'],
                chan_result_dict['CHAN'],
                chan_result_dict['LOC'],
                YEAR,
                JDAYSTR,
                dt.datetime.strptime(str(YEAR)+'-'+JDAYSTR, '%Y-%j').date().isoformat(),
                chan_result_dict['DATA_SECS'],
                chan_result_dict['DATA_PCNT'],
                chankey,
                query_timestamp.isoformat()
                ])

    data_channels.clear()

    if args.verbosity>=2:
        print('Source cnt:', data_count)
        print('Groups cnt:', group_count)



results.sort(key=lambda row: row[RESREC_CHANKEY_NDX])
for row in results:
    if args.verbosity>=2:
        print('Writing result row:',', '.join([str(item) for item in row]))
    fl.write(', '.join([str(item) for item in row]) + '\n')

    # for monthlong overall calculations
    chankey = row[RESREC_CHANKEY_NDX]
    if chankey in monsum.keys():
        monsum[chankey] += row[RESREC_DAY_SECS_NDX]
    else:
        monsum[chankey] = row[RESREC_DAY_SECS_NDX]

for key, val in monsum.items():
    monsum[key] = round(val*100.0/(SECONDS_PER_DAY * month_len), 1)

fl.close()

# set x jday axis
plotdata['x'] = range(JDAY_START,JDAY_START + month_len)

plt.figure(figsize=(7.5,7.5))

#  loop through each day grabbing pcnt for each chankey
for ndx, chankey in enumerate(sorted(monthly_chankey_list)):
    chankey_results = [rec for rec in results if rec[RESREC_CHANKEY_NDX] == chankey]
    chan_code = chankey_results[0][RESREC_CHANNEL_NDX]
    loc_code = chankey_results[0][RESREC_LOC_NDX]
    labelstr = 'Loc: ' + loc_code + ' (' + str(monsum[chankey]) + '%)'

    # initalize all days to 0.0 pcnt
    plotdata[chankey] = [0.0 for jday in range(JDAY_START,JDAY_START + month_len)]

    # have all results for given key, now loop through days
    # setting pcnt to actual value for key
    for rec in chankey_results:
        # print('plotdata-chanlist-x:', plotdata[chankey])
        # print('rec:',rec)
        plotdata[chankey][int(rec[RESREC_JDAYSTR_NDX])-JDAY_START] = rec[RESREC_DAY_PCNT_NDX]

    # print(chankey, plotdata[chankey])
    if chan_code == 'BH1':
        plt.subplot(3,1,1)
        plt.title(STA + ' - ' + chan_code + ": " + cal.month_abbr[int(args.month)] + ', ' + str(YEAR))
    elif chan_code == 'BH2':
        plt.subplot(3,1,2)
        plt.title(STA + ' - ' + chan_code + ": " + cal.month_abbr[int(args.month)] + ', ' + str(YEAR))
    elif chan_code == 'BHZ':
        plt.subplot(3,1,3)
        plt.title(STA + ' - ' + chan_code + ": " + cal.month_abbr[int(args.month)] + ', ' + str(YEAR))

    if loc_code == '00':
        plt.plot([day-0.02 for day in plotdata['x']], plotdata[chankey], color='blue', marker='s', fillstyle='none', linestyle='--', label=labelstr)
    elif loc_code == '10':
        plt.plot([day+0.02 for day in plotdata['x']], plotdata[chankey], color='magenta', marker='x', linestyle=':', label=labelstr)
    elif loc_code == COMBINED_CHANNEL_LOC_CODE:
        plt.plot(plotdata['x'], plotdata[chankey], color='green', marker='*', label=labelstr)

    plt.legend(loc='lower right', frameon=False, fontsize='small')

    # plt.plot(plotdata['x'], plotdata[chankey])
    # plt.set_linestyle('-')
    # plt.set_marker('d')
    plt.ylabel('% Avail')
    plt.xlabel('Ordinal Day')
    plt.axis([JDAY_START-0.5, JDAY_START + month_len-0.5, -10.0, 110.0])
    plt.grid(which='both', axis='x', linestyle=':')
    plt.grid(which='major', axis='y', linestyle=':')

#  lets try and plot
plt.subplots_adjust(hspace=1)
plt.savefig(fname+'.png', dpi=300, format='png')
plt.show()

results.clear()

