# DormantController Wakeup Tool

## Overview

This is a small commandline utility to wake up the dormant controller

It uses the python hid library to send an output report packet to the dormant controller device, so that it will wake up and enumerate as xinput.

## How to run

Install required packages found in requirements.txt then run the .py script. You need `hidapi.dll` in the same folder.