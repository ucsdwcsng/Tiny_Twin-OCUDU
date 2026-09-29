<!-- SPDX-License-Identifier: CC-BY-4.0 -->

### Adding console traces in oai code
#### console messages macros
```C
LOG_E(<component>,<format>,<argument>,...)
LOG_W(<component>,<format>,<argument>,...)
LOG_A(<component>,<format>,<argument>,...)
LOG_I(<component>,<format>,<argument>,...)
LOG_D(<component>,<format>,<argument>,...)
LOG_T(<component>,<format>,<argument>,...)
)
```
these macros are used in place of the printf C function. The additional ***component*** parameter identifies the functional module which generates the message. At run time, the message will only be printed if the configured log level for the component is greater or equal than the macro level used in the code.

| macro | level letter | level value | level name |
|:---------|:---------------|:---------------|----------------:|
| LOG_E |  E | 0 | error |
| LOG_W | W | 1 | warning |
| LOG_A | A | 2 | analysis |
| LOG_I | I | 3 | informational |
| LOG_D | D | 4 | debug |
| LOG_T | T | 5 | trace |

component list is defined as an `enum` in  [log.h](../log.h). A new component can be defined by adding an item in this type, it must also be defined in the T tracer [T_messages.txt ](../../T/T_messages.txt).

Most oai sources are including LOG macros.

#### conditional code macros

```C
LOG_DEBUGFLAG(<flag>)
```
this macro is to be used in if statements. The condition is true if the flag has been set, as described in the [run time usage page](rtusage.md)
```C
if ( LOG_DEBUGFLAG(<flag>) {
/*
   the code below is only executed if the corresponding
   <flag>_debug option is set.
 */
......................
......................
}
```

#### memory dump macros
```C
LOG_DUMPFLAG(<flag>)
```
this macro is to be used in if statements. The condition is true if the flag has been set, as described in the [run time usage page](rtusage.md). It is mainly provided to surround LOG_M macros or direct calls to `log_dump`which otherwise would be unconditionals.
```C
if ( LOG_DUMPFLAG(<flag>) {
/*
   the code below is only executed if the corresponding
   <flag>_dump option is set.
 */
LOG_M(.............
LOG_M(.............
log_dump(...
}

#### matlab format dump
```C
LOG_M(file, vector, data, len, dec, format)
```
|argument| type| description |
|:-----------|:-------|-----------------:|
| file       | char* |path to the fle which will contain the dump |
|vector  |char * |name of the dump, printed at the top of the dump file |
|data| void *| pointer to the memory to be dumpped |
|len |  int | length of the data to be dumpped, in `dec` unit|
| dec| int | length of each data item.Interpretation depends on format|
|format| int | defines the type of data to be dumped|

This macro can be used to dump a buffer in a format that can be used for analyze via tools like matlab or octave. **It must be surrounded by LOG_DEBUGFLAG or LOG_DUMPFLAG macros, to prevent the dump to be built unconditionally.** The LOG_M macro points to the `write_file_matlab` function implemented in  [log.c](../log.c). **This function should be revisited for more understandable implementation and ease of use (format parameter ???)**

#### hexadecimal format dump
```C
LOG_DUMPMSG(c, f, b, s, x...)
```
dumps a memory region if the corresponding debug flag `f` is set as explained here [run time usage page](rtusage.md)

|argument| type| description |
|:-----------|:-------|-----------------:|
| c       | int, component id (in `comp_name_t` enum)| used to print the message, as specified by the s and x arguments |
|f  |int  |flag used to filter the dump depending on the logs configuration. flag list is defined by the LOG_MASKMAP_INIT macro in  [log.h](../log.h) |
|b| void *| pointer to the memory to be dumpped |
|s |  int | length of the data to be dumpped in char|
| x...| printf format and arguments| text string to be printed at the top of the dump|

This macro can be used to conditionaly dump a buffer, bytes by bytes, giving the integer value of each byte in hexadecimal form.

This macro points to the `log_dump` function, implemented in  [log.c](../log.c). This function can also dump buffers containing `double` data via the LOG_UDUMPMSG macro

```C
LOG_UDUMPMSG(c, b, s, f, x...)
```
|argument| type| description |
|:-----------|:-------|-----------------:|
| c       | int, component id (in `comp_name_t` enum)| used to print the message, as specified by the s and x arguments |
|b| void *| pointer to the memory to be dumpped |
|s |  int | length of the data to be dumpped in char|
|f|  int | format of dumped data LOG_DUMP_CHAR or  LOG_DUMP_DOUBLE|
| x...| printf format and arguments| text string to be printed at the top of the dump|

[logging facility developer main page](devusage.md)
[logging facility  main page](log.md)
