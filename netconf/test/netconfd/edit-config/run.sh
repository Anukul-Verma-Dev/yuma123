#!/bin/bash -e
rm -rf tmp || true
mkdir tmp
if [ "$RUN_WITH_CONFD" != "" ] ; then
  cd tmp
  killall -KILL confd || true
  echo "Starting confd: $RUN_WITH_CONFD"
  source $RUN_WITH_CONFD/confdrc
  confdc -c /usr/share/yuma/modules/ietf/ietf-interfaces.yang --yangpath /usr/share/yuma/modules/ietf -o ietf-interfaces.fxs
  confdc -c /usr/share/yuma/modules/ietf/iana-if-type.yang --yangpath /usr/share/yuma/modules/ietf -o iana_if_type.fxs
  NCPORT=2022
  NCUSER=admin
  NCPASSWORD=admin
  confd --verbose --foreground --addloadpath /home/vladimir/transpacket/confd/root/src/confd --addloadpath ${RUN_WITH_CONFD}/src/confd --addloadpath ${RUN_WITH_CONFD}/src/confd/yang --addloadpath ${RUN_WITH_CONFD}/src/confd/aaa --addloadpath . 2>&1 1>server.log &
  SERVER_PID=$!
  cd ..
else
  killall -KILL netconfd || true
  rm /tmp/ncxserver.sock || true
  /usr/sbin/netconfd --module=/usr/share/yuma/modules/ietf/ietf-interfaces.yang --module=/usr/share/yuma/modules/ietf/iana-if-type.yang --no-startup --superuser=$USER 2>&1 1>tmp/server.log &
  SERVER_PID=$!
fi
sleep 3
python session.ncclient.py --server=$NCSERVER --port=$NCPORT --user=$NCUSER --password=$NCPASSWORD
python session.duplicated-list-entry.litenc.py --server=$NCSERVER --port=$NCPORT --user=$NCUSER --password=$NCPASSWORD
python session.replace-leaf.yangcli.py --server=$NCSERVER --port=$NCPORT --user=$NCUSER --password=$NCPASSWORD
kill -KILL $SERVER_PID
cat tmp/server.log
sleep 1
