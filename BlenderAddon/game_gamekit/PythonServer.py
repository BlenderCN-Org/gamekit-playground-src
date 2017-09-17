#!/usr/bin/env python
 
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.
#
 
 
###
### NOT IN USE
### 
 
import bpy
import socket
import asyncio
from asyncio import coroutine, sleep, Task, wait_for
import heapq
import socket
import subprocess
import time
import os
import sys
import json
import struct
import runtimedata
import traceback
import zlib

runtimedata.RuntimeData.debugSession={}
runtimedata.RuntimeData.clientQueue={}

from . import asynciobridge


def addToClientQueue(command):
    runtimedata.RuntimeData.clientQueue.append(command)
    
def startSinglestep(socket):
    def call(s):
        print("SetSinglestep")
        s.mysend(b'SetSinglestep\n')
    addToClientQueue(call)

def stopSinglestep(socket):
    def call(s):
        print("DisableSinglestep")
        s.mysend(b'DisableSinglestep\n')
    addToClientQueue(call)
    
def requestStep(socket):
    def call(s):
        print("RequestStep")
        s.mysend(b'RequestStep\n')
    addToClientQueue(call)



class mysocket:
    '''demonstration class only
      - coded for clarity, not efficiency
    '''

    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))
        self.sock.setblocking(0)

    def mysend(self, msg):
        totalsent = 0
        while totalsent < len(msg):
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent
        
        print("SEND READY! "+str(totalsent))

    def mucki(self):
        print("MUCKI")
    
    def myreceive(self):
        print("rec1")
        chunks = []
        print("rec1")
        bytes_recd = 0
        while bytes_recd < 1000:
            print("rec2")
#             chunk = self.sock.recv(min(1000 - bytes_recd, 2048))
#             if chunk == '':
#                 raise RuntimeError("socket connection broken")
#             chunks.append(chunk)
#             #print("Chunk:"+str(''.join(chunks)))
#             bytes_recd = bytes_recd + len(chunk)
#             print("stuff")
#             print(repr(chunk))
            yield from sleep(0.1)
#             print("rec3")
            
        return ''.join(chunks)




@coroutine
def test_1():
    print("START SERVER TASK")
    s = mysocket()
    print("2")
    s.connect("localhost", 9090)
    print("3")
    print("4")
    print("rec1")
    chunks = []
    print("rec1")
    bytes_recd = 0
    
#     length = -1
#     while length == -1:
#         try:
#             print ("TRY")   
#             chunk = s.sock.recv(4)
#             print("RECEIVED:" + len(chunk))
#             if len(chunk==4):
#                 length = unpack("l",chunk)
#                 print ("FOUND LENGTH:"+length)
#         except:
#             pass
#         
#         yield from sleep(1)
#     
#     if True:
#         return
    world = bpy.data.worlds[0]
    while world.gk_nodetree_debugmode:
        print("--SEND")
#        s.mysend(b'GetDebug\n')
        s.mysend(b'GetDebugCompressed\n')
        compressed = True
        bytes_recd = 0
        chunks = []
        
        
        print("START RECEIVING")
        while world.gk_nodetree_debugmode:
            try:
                #print("--1")
                #print("--RECEIVE:" + str(len(chunks)))

                chunk = s.sock.recv(4096)
                if len(chunk)==0:
                    yield from sleep(0.01)
                    continue
                
                if chunk == '':
                    raise RuntimeError("socket connection broken")
                
                #print("Chunk:" + str(chunk))
                #print("Chuink-Len:"+str(len(chunk)))
                chunks.append(chunk)
                
                endCheck = chunk[-6:]
                if len(endCheck) > 0:
                    print (endCheck)
                if (endCheck==b'##95##'):
                    break
                #print("--2")
            except:
                print(traceback.format_exc())
                e = sys.exc_info()[0]
                print ("ERROR:"+str(e))
                pass
            
            yield from sleep(0.001)
            for command in runtimedata.RuntimeData.clientQueue:
                command(s)
    #             print("rec3")

        print("FINISHED RECEIVING")
        
        if not world.gk_nodetree_debugmode:
            break;
         
        result = b''.join(chunks)
       # print("5:"+result)
        #print("TYPE:"+str(type(result)))
        #print("--RESULT:" + str(len(stringresult)))
        
        stringresult = None
        
        if (compressed):
            before = len(result)
            result = zlib.decompress(result[:-6])
            after = len(result)
            #print("UNCOMPRESSION: from "+str(before)+" to "+str(after))
            #print("UNCOMP:"+str(result))
            stringresult = result.decode("utf-8")
        else:
            stringresult = result[:-6].decode("utf-8")
        #print("RESULT:" + str(stringresult))


        debug_data = json.loads(stringresult)
    
        # first clear all possible Objects (overkill?)
        for tree in bpy.data.node_groups:
            tree.debugPossibleObjects.clear()
        
        # parse incoming debug-data and fill the coresponding props
        for item in debug_data:
#             print ("Tree:" + item.get("treename")+ " Owner:" + item.get("owner"))
            try:
                tree = bpy.data.node_groups[item.get("treename")]
                
                obj = tree.debugPossibleObjects.add()
                obj.name=item.get("owner")
                
                debug_treename = "debug_"+item.get("treename")+"_"+item.get("owner")
                runtimedata.RuntimeData.debugSession[debug_treename]=item.get("nodes")
    #             print ("Tree:" + item.get("treename")+ " Owner:" + item.get("owner"))
                treename = item.get("treename")
                treeowner = item.get("owner")
                showDebugInfo(treename,treeowner)
            except:
                print(traceback.format_exc())

        bpy.ops.wm.redraw_timer(type="DRAW_WIN_SWAP",iterations=1)

#         for node in item.get("nodes"):
#             print ("")
#             print ("\tNode:"+node.get("nodeName"))
#             if ("color" in node.keys()):
#                 print ("\tNODE-Color: " + node.get("color"))
#             
#             print ("Inputs:")
#             for iS in node.get("inputs"):
#                 print("\t\t"+iS.get("socketName")+" => "+iS.get("value"))
#             print ("Outputs:")
#             for oS in node.get("outputs"):
#                 print("\t\t"+oS.get("socketName")+" => "+oS.get("value"))
        
    print("ADIOS SERVER")      
        #print (str(item))

def showDebugInfo(original_treename,objectname):
    debug_treename = "debug_"+original_treename+"_"+objectname
    try:
        nodetree = bpy.data.node_groups[debug_treename]
        debugData = runtimedata.RuntimeData.debugSession[debug_treename]
        
        for nodeData in debugData:
            print("1")
            pos = nodetree.nodes.find(nodeData["nodeName"])
            node = nodetree.nodes[pos]
            if "color" in nodeData:
                node.use_custom_color=True
                node.color=eval(nodeData["color"])
            else:
                node.use_custom_color=False
                
            if pos!=-1:
                #print("DO "+nodeData["nodeName"]+" type:"+nodeData["nodeType"])
                for outSockData in nodeData["outputs"]:
                    #print("2")
                    socketName = outSockData["socketName"][4:]
                    valueType = outSockData["valueType"]
                    value = outSockData["value"]
                    socketPos = node.outputs.find(socketName)
                    #print("3:"+socketName+" pos:"+str(socketPos))
                    
                    if socketPos!=-1:
                        #print("4")
                        socket = node.outputs[socketPos]
                        
                        if valueType=="1":
                            #print("5:"+value+"==true?")
                            try:
                                socket.value=value=="true"
                            except:
                                print(traceback.format_exc())
                                print("PROBLEM with:"+nodeData["nodeName"]+"|"+socketName)

                                
                        elif valueType=="2":
                            socket.value=float(value)
                            #print("SET FLOAT: "+socketName+" = "+str(socket.value))
                        elif valueType=="3":
                            socket.value=int(value)
                        elif valueType=="10":
                            socket.value=value
                
                for inSockData in nodeData["inputs"]:
                    print("2")
                    socketName = inSockData["socketName"][3:]
                    valueType = inSockData["valueType"]
                    value = inSockData["value"]
                    socketPos = node.inputs.find(socketName)
                    print("3:"+socketName+" pos:"+str(socketPos))
                    
                    if socketPos!=-1:
                        print("4")
                        socket = node.inputs[socketPos]
                        
                        if valueType=="1":
                            print("5:"+value+"==true?")
                            try:
                                socket.value=value=="true"
                            except:
                                print(traceback.format_exc())
                                print("PROBLEM with:"+nodeData["nodeName"]+"|"+socketName)
                        elif valueType=="2":
                            socket.value=float(value)
                            #print("SET FLOAT: "+socketName+" = "+str(socket.value))
                        elif valueType=="3":
                            socket.value=int(value)
                        elif valueType=="10":
                            socket.value=value
                            #print("SET STRING:"+value)
            
        
        print("Debug-Data:" + str(debugData))
    except:
        print(traceback.format_exc())
        e = sys.exc_info()[0]
        print ("ERROR:"+str(e))
        


        
