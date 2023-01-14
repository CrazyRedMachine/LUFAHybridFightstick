import PySimpleGUI as sg
import os
import ctypes
import sys
import platform
if platform.system() == 'Windows':
	ctypes.CDLL('.\\hidapi.dll')
import hid
import colorsys

def dict_to_list(dictlist):
	return [item for sublist in [dictlist[key] for key in dictlist] for item in sublist]

def listoflist_to_bytes(listoflist):
	flat_list = [item for sublist in listoflist for item in sublist]
	print('[{}]'.format(', '.join(hex(x) for x in flat_list))) #print command in advanced tab
	return bytes(bytearray(flat_list))
	
def send_to_board():
	data = listoflist_to_bytes([[0], [1], [1], [1], [1], [1], [1], [1], [1]])
	# print(data)
	lights.write(data)
	
############

def open_device(vid, pid):
	itf_list = hid.enumerate(vid, pid) # Note: using pid 0x0000 will match any
	itf_list.sort(key=lambda lst:lst['path']) # Sometimes the interfaces are not installed in correct order
	if len(itf_list) > 0:
		lights = hid.Device(None,None,None,itf_list[0]['path'])
		sliderName = lights.product
	else:
		lights = None
		sliderName = 'Device not found'

	return [sliderName, lights]

sliderName, lights = open_device(0x0f0d, 0x0092)
if lights == None:
	sliderName, lights = open_device(0xd0d0, 0xd0d0)

print("Found device "+sliderName+"")

if lights != None:
	print("Attempt to send wakeup packet")
	send_to_board()