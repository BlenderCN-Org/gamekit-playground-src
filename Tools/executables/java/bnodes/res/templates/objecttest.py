 import bpy
 
# # class where the custom properties for the Collection will be nested
# # (each entry of the collection will derive its Properties from this Class
# # and can hold own values for each property)
 
class PropertyGroup(bpy.types.PropertyGroup):
 pass
 
bpy.utils.register_class(PropertyGroup) 
 
 
bpy.types.Object.mychosenObject = bpy.props.StringProperty()
 
# # create CollectionProperty and link it to the property class
bpy.types.Object.myCollection = bpy.props.CollectionProperty(type=PropertyGroup)
bpy.types.Object.myCollection_index = bpy.props.IntProperty(min=-1, default=-1)
# # create Properties for the collection entries:
PropertyGroup.mystring = bpy.props.StringProperty()
PropertyGroup.mybool = bpy.props.BoolProperty(default=False)
 
 
 
# # create operator to add or remove entries to/from  the Collection
class OBJECT_OT_add_remove_Collection_Items(bpy.types.Operator):
 bl_label = "Add or Remove"
 bl_idname = "collection.add_remove"
 __doc__ = "Simple Custom Button"
 
 
 
 add = bpy.props.BoolProperty(default=True)
 
 def invoke(self, context, event):
  add = self.add
  obj = context.object
  collection = obj.myCollection
  if add:
   collection.add()  # This add at the end of the collection list
  else:
   index = obj.myCollection_index
   collection.remove(index)  # This remove on item in the collection list function of index value
   
 return {'FINISHED'} 
 
class OBJECT_PT_ObjectSelecting(bpy.types.Panel):
 
 bl_label = "Object Selecting"
 bl_space_type = "PROPERTIES"
 bl_region_type = "WINDOW"
 bl_context = "object"
 
 def draw(self, context):
  obj = context.object
  layout = self.layout
  
  # #show collection in Panel:
  row = layout.row()
  row.template_list(obj, "myCollection", obj, "myCollection_index")  # This show list for the collection
  # #show add/remove Operator
  col = row.column(align=True)
  col.operator("collection.add_remove", icon="ZOOMIN", text="")  # This show a plus sign button
  col.operator("collection.add_remove", icon="ZOOMOUT", text="").add = False  # This show a minus sign button
  
  # #change name of Entry:
  if obj.myCollection:
   entry = obj.myCollection[obj.myCollection_index]
   layout.prop(entry, "name")
   
   # #show self created properties of myCollection
   layout.prop(entry, "mystring")
   layout.prop(entry, "mybool")
  
  # ## search prop to search in myCollection:
  layout.prop_search(obj, "mychosenObject", obj, "myCollection")
    
    
bpy.utils.register_class(OBJECT_PT_ObjectSelecting) 
bpy.utils.register_class(OBJECT_OT_add_remove_Collection_Items) 
# bpy.utils.register_class(PropertyGroup)
