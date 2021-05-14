# -*- coding: utf-8 -*-
"""
Created on Fri May 14 10:59:33 2021

@author: danny
"""

def parseBetween(data_in='',start='<reply>',stop='</reply>'):
    index_start = data_in.find(start)
    index_stop  = data_in.find(stop)
    
    if index_start == -1:
        index_start = 0
        
    if index_stop == -1:
        index_stop = len(data_in)-1
        
    if index_start+len(start) >= len(data_in[:index_stop]):
        index_start = 0
    
    return data_in[index_start+len(start):index_stop]

def send(my_socket,request_id=0):
    my_socket.sendall(str.encode('<request>{}</request>'.format(request_id)))
    
def decodeData(data_in):
    data = []
    
    i = 0
    start_index = 0
    stop_index = 0
    
    while i < len(data_in):
        if data_in[i] == '{':
            start_index = i
            
            while data_in[i] != '}':
                i += 1
                
            stop_index = i+1
        i += 1
            
        data.append(int(parseBetween(data_in=data_in[start_index:stop_index],start='{',stop='}')))
        
    return data           
        
def isRequestValid(data_in=''):
    if len(data_in) < len('<request-valid?>') + len('</request-valid?>'):
        return False
    
    if data_in.find('<request-valid?>') == -1 or data_in.find('</request-valid?>') == -1:
        return False
    
    return int(parseBetween(data_in=data_in,start='<request-valid?>',stop='</request-valid?>'))
    
def readAll(my_socket):
    data = my_socket.recv(1024).decode('utf8')
    
    while data.find('</reply>') == -1:
        data = data + my_socket.recv(1024).decode('utf8')
                
    return parseBetween(data_in=data,start='<reply>',stop='</reply>')

def startAcquisition(my_socket):
    request_id = 0
    
    send(my_socket=my_socket,request_id=request_id)
    
    reply = readAll(my_socket=my_socket)
       
    return isRequestValid(data_in=reply)

def stopAcquisition(my_socket):
    request_id = 1
    
    send(my_socket=my_socket,request_id=request_id)
    
    reply = readAll(my_socket=my_socket)
       
    return isRequestValid(data_in=reply)

def isAcquisitionRunning(my_socket):
    request_id = 2
    
    send(my_socket=my_socket,request_id=request_id)
    
    reply = readAll(my_socket=my_socket)
    
    valid = isRequestValid(data_in=reply)
    
    running = False
    
    if valid:
        running = parseBetween(data_in=reply,start='<reply-data>',stop='</reply-data>')
        
    return running
    
# returns the number of counts for a specific spectrum ...
def getCounts(my_socket,spectra_type='AB'):
    request_id = False
    
    if spectra_type == 'AB':
        request_id = 12
    elif spectra_type == 'BA':
        request_id = 13
    elif spectra_type == 'merged':
        request_id = 14
    elif spectra_type == 'prompt':
        request_id = 15
    else:
        assert request_id == True
    
    send(my_socket=my_socket,request_id=request_id)
    
    reply = readAll(my_socket=my_socket)
    
    valid = isRequestValid(data_in=reply)
    
    counts = 0
    
    if valid:
        counts = parseBetween(data_in=reply,start='<reply-data>',stop='</reply-data>')
        
    return int(counts)

def getCountsOfABSpectrum(my_socket):
    return getCounts(my_socket=my_socket,spectra_type='AB')  

def getCountsOfBASpectrum(my_socket):
    return getCounts(my_socket=my_socket,spectra_type='BA')

def getCountsOfMergedSpectrum(my_socket):
    return getCounts(my_socket=my_socket,spectra_type='merged')

def getCountsOfPromptSpectrum(my_socket):
    return getCounts(my_socket=my_socket,spectra_type='prompt')  

def resetSpectrum(my_socket,spectra_type='AB'):
    request_id = False
    
    if spectra_type == 'all':
        request_id = 3
    elif spectra_type == 'AB':
        request_id = 4
    elif spectra_type == 'BA':
        request_id = 5
    elif spectra_type == 'merged':
        request_id = 6
    elif spectra_type == 'prompt':
        request_id = 7
    else:
        assert request_id == True
        
    send(my_socket=my_socket,request_id=request_id)
    
    reply = readAll(my_socket=my_socket)
       
    return isRequestValid(data_in=reply)

def resetAllSpectra(my_socket):
    return resetSpectrum(my_socket=my_socket,spectra_type='all')

def resetABSpectrum(my_socket):
    return resetSpectrum(my_socket=my_socket,spectra_type='AB')

def resetBASpectrum(my_socket):
    return resetSpectrum(my_socket=my_socket,spectra_type='BA')

def resetMergedSpectrum(my_socket):
    return resetSpectrum(my_socket=my_socket,spectra_type='merged')

def resetPromptSpectrum(my_socket):
    return resetSpectrum(my_socket=my_socket,spectra_type='prompt')

def getData(my_socket,spectra_type='AB'):
    request_id = False
    
    if spectra_type == 'AB':
        request_id = 8
    elif spectra_type == 'BA':
        request_id = 9
    elif spectra_type == 'merged':
        request_id = 10
    elif spectra_type == 'prompt':
        request_id = 11
    else:
        assert request_id == True
        
    send(my_socket=my_socket,request_id=request_id)
    
    reply = readAll(my_socket=my_socket)
    
    valid = isRequestValid(data_in=reply)
    
    data_dict = {
                "channel-width": 0.0,
                "no-of-channel": 0,
                "integral-counts": 0,
                "spectrum-data": []
                }
    
    if valid:
        reply_data = parseBetween(data_in=reply,start='<reply-data>',stop='</reply-data>')
        
        channel_width   = parseBetween(data_in=reply_data,start='<channel-width-ps>',stop='</channel-width-ps>')
        no_of_channel   = parseBetween(data_in=reply_data,start='<number-of-channel>',stop='</number-of-channel>')
        integral_counts = parseBetween(data_in=reply_data,start='<integral-counts>',stop='</integral-counts>')
        
        lt_data = parseBetween(data_in=reply_data,start='<data>',stop='</data>')
        
        spectrum = decodeData(lt_data)
        
        data_dict["channel-width"] = channel_width
        data_dict["no-of-channel"] = no_of_channel
        data_dict["integral-counts"] = integral_counts
        data_dict["spectrum-data"] = spectrum
    
    return data_dict

def getDataOfABSpectrum(my_socket):
    return getData(my_socket=my_socket,spectra_type='AB')

def getDataOfBASpectrum(my_socket):
    return getData(my_socket=my_socket,spectra_type='BA')

def getDataOfMergedSpectrum(my_socket):
    return getData(my_socket=my_socket,spectra_type='merged')

def getDataOfPromptSpectrum(my_socket):
    return getData(my_socket=my_socket,spectra_type='prompt')
    

import socket
import matplotlib.pyplot as plt

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 4000        # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    
    resetABSpectrum(my_socket=s)
    
    while True:
        resetABSpectrum(my_socket=s)
        
        while getCountsOfABSpectrum(my_socket=s) < 1000:
            a = 1

        spectrum_data = getDataOfABSpectrum(my_socket=s)
    
        spectrum = spectrum_data["spectrum-data"]
        print(spectrum_data["channel-width"])
        print(spectrum_data["no-of-channel"])
        print(spectrum_data["integral-counts"])
    
        plt.semilogy(spectrum,'bo')
        plt.show()
    
    #getData(my_socket=s,spectra_type='AB')
    
    
    #s.sendall(b'<request>8</request>')
    #data = repr(s.recv(1024))
    
    #while data.find('</reply>') == -1:
    #    data = data + repr(s.recv(1024))

#print('Received', repr(data))