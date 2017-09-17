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
 
import protocol
import asyncio
import traceback

import zlib

import bpy
import socket
import asyncio
import json
import struct
import runtimedata
import traceback
import zlib,sys


requestScreenshotOngoing = False


def vec2dict(vec):
    return [vec.x,vec.y,vec.z]

def matrix2dict(mat):
    return b"Gk.Matrix4(%s,%s,%s,%s)" % (vec4_2lua(mat[0]),vec4_2lua(mat[1]),vec4_2lua(mat[2]),vec4_2lua(mat[3]))

def vec4_2lua(vec):
    return b"Gk.Vector4(%f,%f,%f,%f)" % (vec.x,vec.y,vec.z,vec.w)

def vec2lua(vec):
    return b"Gk.Vector3(%f,%f,%f)" % (vec.x,vec.z,vec.y)

def quat2lua(quat):
    return b"Gk.Quaternion(%f,%f,%f,%f)" %(quat.w,quat.x,quat.y,quat.z)

@asyncio.coroutine
def coSetBlenderData(vmat,proj,near,far,ortho):
    try:
        print("_getScreenshot")

        if protocol.client.writer:
            try:
                yield from protocol.client.startSession()
                lua = b"LuaCall|setData(%s,%s)  " % (matrix2dict(vmat),matrix2dict(proj))
                print("pyLUA:" + str(lua)) 
                protocol.writeMessage(protocol.client.writer
                                      ,protocol.Kind.lua
                                      ,lua)                
                
                (kind,raw) = yield from protocol.readMessage(protocol.client.reader)
                print("Data-Response:" + str(raw))
                print("###############################################OK DATA SENT!")
            except:
                print("2")
                print(traceback.format_exc())
                #protocol.client.close()
                raw="__nothing__"
                RUNNING=False
    finally:
        yield from protocol.client.endSession()
        pass
    return "finished sentdata:"+str(raw)
#         output = {}
#         output["pos"]=vec2dict(pos)
#         output["rot"]=vec2dict(rot)
#         output["proj"]=[vec2dict(proj[0]),vec2dict(proj[1]),vec2dict(proj[2]),vec2dict(proj[3])]
#         output["near"]=near
#         output["far"]=far
#         output["ortho"]=ortho
#         
#         json = json.dumps(output)

        
    


import asynciobridge


def singleStep(callType):
    @asyncio.coroutine
    def serverCall(callType):
        print("HALLO")
        try:
            print("_singlstep:" + callType)
    
            if protocol.client.writer:
                try:
                    yield from protocol.client.startSession()
                    if (callType == "start"):
                        lua = b"LuaCall|Gk.setSinglestepMode(true)  ";
                    elif (callType=="stop"):
                        lua = b"LuaCall|Gk.setSinglestepMode(false)  ";
                    elif (callType=="step"):
                        lua = b"LuaCall|Gk.requestSinglestep()  ";
                    else:
                        print("Unknown SingleStepType:"+callType)
                        return
                    
                    print("pyLUA:" + str(lua)) 
                    protocol.writeMessage(protocol.client.writer
                                          ,protocol.Kind.lua
                                          ,lua)                
                    
                    (kind,raw) = yield from protocol.readMessage(protocol.client.reader)
                    print("Data-Response:" + str(raw))
                    print("###############################################OK DATA SENT!")
                except:
                    print("2")
                    print(traceback.format_exc())
                    #protocol.client.close()
                    raw="__nothing__"
                    RUNNING=False
        finally:
            yield from protocol.client.endSession()
            pass
                 
    asyncio.Task(serverCall(callType))   
    print("AFTER")     
        
        

def requestScreenshot(width,height,vmat, projection, near, far, is_ortho,callback):
    global requestScreenshotOngoing
    
    if requestScreenshotOngoing:
        print("ALREADY REQUESTING")
        return
    
    
    
    print ("REQUEST SCREENSHOT")
    requestScreenshotOngoing = True
    
    @asyncio.coroutine
    def _getScreenshotFromGamekit(width,height,vmat, projection, near, far, is_ortho):
        try:
            print("_getScreenshot")
            try:
                yield from protocol.client.connect("127.0.0.1",9090)
                print ("CONNECTED")
            except:
                print ("PIPE ERROR")
                protocol.client.close()
                print ("wait...")
    
            try:
                
                result = yield from coSetBlenderData(vmat, projection, near, far, is_ortho)
                print ("SET BLEND-RESULT:"+str(result))
                
                print("Write")
                yield from protocol.client.startSession()
                protocol.writeMessage(protocol.client.writer
                                      ,protocol.Kind.command
                                      ,protocol.Commands.GK_SCREENSHOT_REQUEST+b"|"+str(width).encode("utf-8")+b"|"+str(height).encode("utf-8"))
                print("READ")
                (kind,raw) = yield from protocol.readMessage(protocol.client.reader)
                before = len(raw)
                uncompRAW = zlib.decompress(raw)
                after = len(result)
                print( ("BEFORE:%i After:%i") % (before,after) )
                yield from protocol.client.endSession()
                print("###############################################SCREEN RES:"+str(width)+"x"+str(height))
                #print("SCREENSHOT READ-Success:" + str(raw))
                #callback(width,height,raw)
                callback(width,height,uncompRAW)
                bpy.ops.wm.redraw_timer(type="DRAW_WIN_SWAP",iterations=1)
            except:
                print(traceback.format_exc())
                protocol.client.close()
                raw="__nothing__"
                RUNNING=False
                yield from protocol.client.endSession()
        finally:
            global requestScreenshotOngoing
            requestScreenshotOngoing = False

    
    asyncio.Task(_getScreenshotFromGamekit(width,height,vmat, projection, near, far, is_ortho))

runtimedata.RuntimeData.DEBUG_POLLING = False

def stopDebugPolling():
    runtimedata.RuntimeData.DEBUG_POLLING = False
    
def startDebugPolling():
    
    if (runtimedata.RuntimeData.DEBUG_POLLING):
        print("POLLING IS ALREADY RUNNING")
        return
    
    runtimedata.RuntimeData.DEBUG_POLLING=True
        
    print("START DEBUG-Polling")
    @asyncio.coroutine
    def debugCoroutine():
        kind = 0
        raw = "__err__"
        print("IN MY UPDATE!!!!!! ")
        
        
        while runtimedata.RuntimeData.DEBUG_POLLING:
            try:
                yield from protocol.client.connect("127.0.0.1",9090)
                print ("CONNECTED")
            except:
                print ("PIPE ERROR")
                protocol.client.close()
                bpy.data.worlds[0].gk_nodetree_debugmode = False
                print ("wait...")
    
                    
            try:
                print("Write")
                yield from protocol.client.startSession()
                protocol.writeMessage(protocol.client.writer
                                      ,protocol.Kind.command
                                      ,protocol.Commands.NODETREE_DEBUG_PACKED)
                print("READ")
                (kind,raw) = yield from protocol.readMessage(protocol.client.reader)
                yield from protocol.client.endSession()
                print("READ-Success:" + str(raw))
                ProcessDebugData(True,raw)
            except:
                print(traceback.format_exc())
                protocol.client.close()
                raw="__nothing__"
                RUNNING=False
                yield from protocol.client.endSession()
                
            print ("Received: kind"+str(kind))
            print ("Raw:"+str(raw))
            
            yield from asyncio.sleep(0.5)
             
    asyncio.Task(debugCoroutine())

def ProcessDebugData(compressed,data):
    stringresult=None
    if (compressed):
        before = len(data)
        result = zlib.decompress(data)
        after = len(result)
        print("UNCOMPRESSION: from "+str(before)+" to "+str(after))
        #print("UNCOMP:"+str(result))
        stringresult = result.decode("utf-8")
    else:
        stringresult = result.decode("utf-8")
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

def setSocketValue(socket,value):
    print("Socket:"+socket.name+" type:"+str(socket.rna_type.identifier));
    if socket.rna_type.identifier=="NodeTreeSTRING":
        socket.value=str(value)
    else:
        socket.value=value

def showDebugInfo(original_treename,objectname):
    debug_treename = "debug_"+original_treename+"_"+objectname
    try:
        nodetree = bpy.data.node_groups[debug_treename]
        debugData = runtimedata.RuntimeData.debugSession[debug_treename]
        
        for nodeData in debugData:
            try:
                print("1")
                pos = nodetree.nodes.find(nodeData["nodeName"])
                node = nodetree.nodes[pos]
                if "STATUS" in nodeData:
                    node.STATUS=nodeData["STATUS"]
                else:
                    node.STATUS=""
                    
                if "color" in nodeData:
                    node.use_custom_color=True
                    node.color=eval(nodeData["color"])
                else:
                    node.use_custom_color=False
                    
                if pos!=-1:
                    for inSockData in nodeData["inputs"]:
                        print("2")
                        socketName = inSockData["socketName"][3:]
                        valueType = inSockData["valueType"]
                        value = inSockData["value"]
                        socketPos = node.inputs.find(socketName)
                        print(node.name+"3:"+socketName+" pos:"+str(socketPos)+" valuetype:"+valueType)
                        
                        if socketPos!=-1:
                            print("4")
                            socket = node.inputs[socketPos]
    
                            try:                        
                                if valueType=="1":#bool
                                    print("5:"+value+"==true?")
                                    setSocketValue(socket,(value=="true"))
                                elif valueType=="2": #float
                                    setSocketValue(socket,float(value))
                                    #print("SET FLOAT: "+socketName+" = "+str(socket.value))
                                elif valueType=="3": # int
                                    setSocketValue(socket,int(value))
                                elif valueType=="4" or valueType=="5" or valueType=="6": # vec2,vec3,vec4
                                    print ("###############################################################################################")
                                    print ("###############################################################################################")
                                    print ("###############################################################################################")
                                    print ("###############################################################################################")
                                    #socket.value=eval(value)
                                    setSocketValue(socket,eval(value))
                                elif valueType=="10": #string
                                    socket.value=value
                                    #print("SET STRING:"+value)
                            except:
                                print(traceback.format_exc())
                                print("PROBLEM with:"+nodeData["nodeName"]+"|"+socketName)
                
                #print("DO "+nodeData["nodeName"]+" type:"+nodeData["nodeType"])
                for outSockData in nodeData["outputs"]:
                    #print("2")
                    socketName = outSockData["socketName"][4:]
                    valueType = outSockData["valueType"]
                    value = outSockData["value"]
                    socketPos = node.outputs.find(socketName)
                    #print("3:"+socketName+" pos:"+str(socketPos))

                    print(node.name+"3:"+socketName+" pos:"+str(socketPos)+" valuetype:"+valueType+" value:"+str(value))
                    
                    if socketPos!=-1:
                        #print("4")
                        socket = node.outputs[socketPos]
                        
                        if valueType=="1":
                            #print("5:"+value+"==true?")
                            try:
                                setSocketValue(socket,(value=="true"))
                            except:
                                print(traceback.format_exc())
                                print("PROBLEM with:"+nodeData["nodeName"]+"|"+socketName)

                                
                        elif valueType=="2":
                            setSocketValue(socket,float(value))
                            #print("SET FLOAT: "+socketName+" = "+str(socket.value))
                        elif valueType=="3":
                            try:
                                setSocketValue(socket,int(value))
                            except:
                                print(traceback.format_exc())
                        elif valueType=="4" or valueType=="5" or valueType=="6": # vec2,vec3,vec4
                            print ("##################")
                            try:
                                setSocketValue(socket,eval(value))
                            except:
                                print(traceback.format_exc())
                        elif valueType=="10":
                            socket.value=value
                
            except:
                print(traceback.format_exc())

            
        print("Debug-Data:" + str(debugData))
    except:
        print(traceback.format_exc())
        e = sys.exc_info()[0]
        print ("ERROR:"+str(e))

    

    

    