#!/bin/bash -e
rm -rf tmp
mkdir tmp
cp startup-cfg.xml tmp
/usr/sbin/netconfd --module=test-multiple-edit-callbacks-a --module=test-multiple-edit-callbacks-b --module=iana-if-type --startup=tmp/startup-cfg.xml --superuser=$USER 1>tmp/netconfd.stdout 2>tmp/netconfd.stderr &
NETCONFD_PID=$!
sleep 1
python session.py
kill $NETCONFD_PID
cat /tmp/netconf.stdout
