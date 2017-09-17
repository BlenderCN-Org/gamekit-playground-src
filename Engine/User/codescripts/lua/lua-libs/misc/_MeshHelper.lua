--[[
Usage:

Vertices = 
{
    [0] = {
        pos = {0,0,0},
        uv = {
                {0.0,0.0}
             }
        }
   ,...

}

Tris = {
    {0,1,2}, {1,2,3}
}

mesh = MeshHelper("MeshEntity")
mesh:setVertices(Vertices)
mesh:setTriangles(Tris)
mesh:setMaterialName("Material")
entity = mesh:create()
--]]

MeshHelper = Class()
    
function MeshHelper:constructor(meshName)
    self.scene = OgreKit.getActiveScene()
    self.entity = self.scene:createEntity(meshName)
    self.mesh  = self.entity:getMesh()
    self.subm  = self.mesh:getSubMesh(0);
    self.material = self.subm:getMaterial()
    self.uvSet = false
    self.vertices = nil
    self.tris = nil
    self.created = false
end

function MeshHelper:setVertices(verts)
    self.vertices = verts
end

function MeshHelper:setTriangles(triangles)
    self.tris = triangles
end

function MeshHelper:setMaterialName(matName)
    self.material.m_name = matName
end

function MeshHelper:getMaterialName()
    return self.material.m_name
end

function MeshHelper:create()
    if (self.created) then
        dPrintf("Mesh is already created!")
        return self.entity
    end
    
    for i,v in pairs(self.vertices) do 
        local gkVert = OgreKit.gkVertex()
        v.vert = gkVert;
        v.idx = {}
        
        gkVert.co.x = v.pos[1]
        gkVert.co.y = v.pos[2]
        gkVert.co.z = v.pos[3]
        
        if (#v.uv>0) then
            -- set uv-layer-amount in the submesh
            if (not self.uvSet) then
                self.subm:setUVCount(#v.uv)  
            end
            
            for j,uv in ipairs(v.uv) do
                local gkUV = gkVert:getUV(j-1);
                gkUV.x = uv[1]
                gkUV.y = uv[2]
            end 
        end       
    end
    
    for i,tri in ipairs(self.tris) do
        local v0 = self.vertices[tri[1]]    
        local v1 = self.vertices[tri[2]]
        local v2 = self.vertices[tri[3]]
        
        local normal = Gk.getTriNormal(v0.vert,v1.vert,v2.vert)
        v0.vert.no=normal        
        v1.vert.no=normal
        v2.vert.no=normal

        triIdx = self.subm:addTriangle(v0.vert,tri[1],v1.vert,tri[2],v2.vert,tri[3])
        -- save the indices and the triangles that vertex is used for
        -- might be an overkill
        v0.idx[triIdx.i0]=triIdx
        v1.idx[triIdx.i1]=triIdx
        v2.idx[triIdx.i2]=triIdx
    
    end
    
    self.entity:createInstance()
    self.created = true
    return self.entity
end

function MeshHelper:checkNullVec(v)
    if v.x==0 and v.y==0 and v.z==0 then
        return true
    else
        return false
    end
end

