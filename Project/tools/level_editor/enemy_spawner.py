import bpy

class MYADDON_OT_create_spawn_normalenemy(bpy.types.Operator): 
    bl_idname = "myaddon.create_spawn_normalenemy" 
    bl_label = "通常敵出現ポイント作成" 
    bl_description = "通常敵の出現ポイントを作成します" 
     
    def execute(self, context): 
        bpy.ops.myaddon.create_spawn_symbol('EXEC_DEFAULT', type="NormalEnemy") 
        return {'FINISHED'} 
     
class MYADDON_OT_create_spawn_strongEnemy(bpy.types.Operator): 
    bl_idname = "myaddon.create_spawn_strongenemy" 
    bl_label = "強敵出現ポイント作成" 
    bl_description = "強敵の出現ポイントを作成します" 
 
    def execute(self, context): 
        bpy.ops.myaddon.create_spawn_symbol('EXEC_DEFAULT', type="StrongEnemy") 
        return {'FINISHED'}

