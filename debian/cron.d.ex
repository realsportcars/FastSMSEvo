#
# Regular cron jobs for the fastsms package
#
0 4	* * *	root	[ -x /usr/bin/fastsms_maintenance ] && /usr/bin/fastsms_maintenance
