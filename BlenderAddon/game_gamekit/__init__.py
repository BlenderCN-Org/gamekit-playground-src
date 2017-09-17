#Copyright (c) 2010 Xavier Thomas (xat)
#
# ***** BEGIN MIT LICENSE BLOCK *****
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.
#
# ***** END MIT LICENCE BLOCK *****




bl_info = {
    'name': 'Gamekit Engine',
    'author': 'Xavier Thomas (xat)',
    'version': (0,0,601),
    'api': 35899,
    'blender': (2, 5, 7),
    'location': 'Info Window > Render Engine > Gamekit',
    'description': 'Launch game using the fine gamekit engine',
    'wiki_url': 'http://wiki.blender.org/index.php/Extensions:2.5/Py/Scripts/Gamekit_Engine',
    'tracker_url': 'https://projects.blender.org/tracker/index.php?func=detail&aid=23378&group_id=153&atid=514',
    'category': 'Game Engine'}

import os,sys,traceback

try:
    print("TRY TO START DEBU")
    import pydevd
    pydevd.settrace(stdoutToServer=True, stderrToServer=True, suspend=False)
    print("Started pydevd")
except ImportError:
    print (traceback.format_exc())
    pass


script_file = os.path.realpath(__file__)
directory = os.path.dirname(script_file)
if directory not in sys.path:
    sys.path.append(directory)
        
print ("GAMEKIT ADDON:" + directory)

if "bpy" in locals():
    import imp
    #if "operators" in locals():
    imp.reload(operators)
    imp.reload(ui)
    imp.reload(nodeBookmarks)
    imp.reload(asynciobridge)
    imp.reload(SpaceContext)
    #imp.reload(PythonServer)
    imp.reload(NodetreeDebugger)
    imp.reload(nodetree)
#     imp.reload(pydna)
else:
    from . import operators
    from . import ui
    from . import nodeBookmarks
    from . import asynciobridge
    from . import SpaceContext
    from . import nodetree
    #from . import PythonServer
    from . import NodetreeDebugger
    from . import bpointers
    
#     import pydna
#init_data = True

import bpy

def register():
    #bpy.utils.register_module(__name__)
    SpaceContext.register()
    operators.register()
    bpy.utils.register_class(asynciobridge.AsyncioBridgeOperator)
    ui.addProperties()
    nodeBookmarks.register()
    nodetree.register()
    
    
#     def _get_spacenode_dna(obj):
#        _dna = pydna.types.SpaceNode.from_address(obj.as_pointer())
#        return _dna
   
#     bpy.types.SpaceNodeEditor._dna = property(_get_spacenode_dna)


def unregister():   
    #bpy.utils.unregister_module(__name__)
    
    asynciobridge.stopAsyncServer()
    bpy.utils.unregister_class(asynciobridge.AsyncioBridgeOperator)
    
    operators.unregister()
    
    ui.remProperties()
    nodeBookmarks.unregister()
    SpaceContext.unregister()
        



if __name__ == "__main__":
    register()
