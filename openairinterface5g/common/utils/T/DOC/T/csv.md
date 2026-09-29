<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# csv

This tracer is used to dump information of a single trace to a CSV.

The basic usage is:

```shell
cd common/utils/T/tracer
./csv -d ../T_messages.txt <TRACE> <fields>
```

The available options include `-t` to print a timestamp (from the event
timestamp, i.e., it also works with trace files procuded by `record`, showing
the time when the event has been recorded initially) and `-s` to specify a
separator between fields (use `-s $'\t'` to pass a TAB character; this is
shell dependent, may be different on your setup).

To print a timestamp, you choose the name with `-t` and use the chosen
name in `<fields>`.

For example, with the gNB trace `GNB_PHY_DL_TICK`, if you run:

```
./csv -d ../T_messages.txt GNB_PHY_DL_TICK frame slot
```

You get the following output:

```
frame,slot
974,12
974,13
974,14
974,15
974,16
974,17
974,18
974,19
975,0
[...]
```

Now, to add a timestamp, you can run:

```
./csv -d ../T_messages.txt -t time GNB_PHY_DL_TICK time frame slot
```

Producing:

```
time,frame,slot
11:12:30.894336,49,17
11:12:30.894856,49,18
11:12:30.895341,49,19
11:12:30.895864,50,0
11:12:30.896394,50,1
11:12:30.896875,50,2
11:12:30.897378,50,3
11:12:30.897876,50,4
11:12:30.898427,50,5
11:12:30.898876,50,6
[...]
```

You can use whatever name you want and you can print the timestamp at any position.

For example, running (we also set the separator as `/` instead of the default `,`):

```
./csv -d ../T_messages.txt -s '/' -t ts GNB_PHY_DL_TICK frame ts slot
```

Prints:

```
frame/ts/slot
100/11:20:14.525125/6
100/11:20:14.525650/7
100/11:20:14.526137/8
100/11:20:14.526616/9
100/11:20:14.527116/10
100/11:20:14.527614/11
100/11:20:14.528142/12
100/11:20:14.528648/13
100/11:20:14.529130/14
[...]
```

Note: to generate those traces, the gNB was run as:

```
sudo ./nr-softmodem -E -O ~/b78.conf --T_stdout 2
```

The important part is `--T_stdout 2` to activate the T tracer and allow
`csv` to collect logs.

For more help, run:
```shell
./csv -h
```
