# Echo Commands

A commands library for the Arduino boards, which must communications with any kind foreign platforms. 


**Introduction**

***
Usually i use the Arduino as an intermediate element when i work with complex systems. On the one hand i have a consumer program that sends commands to the Arduino and itself processes these commands and performs some action.
 
**The library can**
 - sending and receiving of commands with arguments
 - attaching callback functions any received command
 

## **The message format**


The message format is a simple way for send and receive of commands. Each of the message is consist of the following items:

## `Command ID` | `param1` | ..... | `param N`

The Message format supports any primary data types, and zero to many multiple arguments.

## **How Echo-Commands wokrs?**

This example shows how to use Echo-Commands as a Arduino library and communicate with it using the any kind of communication interface.

## **Available commands**

**The PC site**
```
1 | <led state>      - Sets led. 0 = off, 1 = on

2 | <led brightness> - Sets led brighness. 0 - 3200

3 |                  - Shows led state
```

**The Arduino board** 
```             
> Command #3
>    Led status: off
>    Led brightness: 0

> Command #1 | 1
>    Led status: on
>    Led brightness: 3200

> Command #3
>   Led status: on
>   Led brightness: 3200

> Command #2 | 200
>   Led status: on
>   Led brightness: 200

> Command #3
>   Led status: on
>   Led brightness: 200
```

