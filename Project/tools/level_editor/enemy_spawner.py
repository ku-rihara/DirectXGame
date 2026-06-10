import bpy

class MYADDON_OT_create_spawn_EntourageEnemy(bpy.types.Operator): 
    bl_idname = "myaddon.create_spawn_EntourageEnemy" 
    bl_label = "通常敵出現ポイント作成" 
    bl_description = "通常敵の出現ポイントを作成します" 
     
    def execute(self, context): 
        bpy.ops.myaddon.create_spawn_symbol('EXEC_DEFAULT', type="EntourageEnemy") 
        return {'FINISHED'} 
     
class MYADDON_OT_create_spawn_LeaderEnemy(bpy.types.Operator): 
    bl_idname = "myaddon.create_spawn_LeaderEnemy" 
    bl_label = "強敵出現ポイント作成" 
    bl_description = "強敵の出現ポイントを作成します" 
 
    def execute(self, context): 
        bpy.ops.myaddon.create_spawn_symbol('EXEC_DEFAULT', type="LeaderEnemy") 
        return {'FINISHED'}

