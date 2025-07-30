import bpy

# ============================
# プリミティブの種類
# ============================
primitive_items = [
    ('PLANE', "Plane", ""),
    ('SPHERE', "Sphere", ""),
    ('CYLINDER', "Cylinder", ""),
    ('RING', "Ring", ""),
    ('BOX', "Box", "")
]

# ============================
# Emitter
# ============================
class EmitterItem(bpy.types.PropertyGroup):
    particle_name: bpy.props.StringProperty(
        name="Particle Name", 
        description="パーティクルの名前",
        default="Particle"
    )
    primitive_type: bpy.props.EnumProperty(
        name="Primitive Type", 
        description="プリミティブの種類",
        items=primitive_items, 
        default='PLANE'
    )

# ============================
# Emitterを追加
# ============================
class OBJECT_OT_add_emitter(bpy.types.Operator):
    bl_idname = "object.add_emitter"
    bl_label = "Emitter追加"
    bl_description = "選択されたオブジェクトにEmitterを追加します"

    def execute(self, context):
        obj = context.object
        if obj is None:
            self.report({'ERROR'}, "オブジェクトが選択されていません")
            return {'CANCELLED'}
        
        # emitter_listプロパティが存在するかチェック
        if not hasattr(obj, 'emitter_list'):
            self.report({'ERROR'}, "Emitterプロパティが初期化されていません。アドオンを再有効化してください。")
            return {'CANCELLED'}
        
        new_emitter = obj.emitter_list.add()
        new_emitter.particle_name = f"Particle_{len(obj.emitter_list)}"
        new_emitter.primitive_type = 'PLANE'
        
        # デバッグ用出力
        print(f"新しいEmitterを追加: {new_emitter.particle_name}, {new_emitter.primitive_type}")
        
        self.report({'INFO'}, f"Emitterを追加しました: {new_emitter.particle_name}")
        return {'FINISHED'}

# ============================
# 指定されたEmitterを削除
# ============================
class OBJECT_OT_remove_emitter(bpy.types.Operator):
    bl_idname = "object.remove_emitter"
    bl_label = "Emitter削除"
    bl_description = "指定されたインデックスのEmitterを削除します"
    bl_options = {'REGISTER', 'UNDO'}

    index: bpy.props.IntProperty(
        name="Index", 
        description="削除するEmitterのインデックス",
        default=0,
        min=0
    )

    def execute(self, context):
        obj = context.object
        if obj is None:
            self.report({'ERROR'}, "オブジェクトが選択されていません")
            return {'CANCELLED'}
        
        if not hasattr(obj, 'emitter_list'):
            self.report({'ERROR'}, "Emitterプロパティが見つかりません")
            return {'CANCELLED'}
        
        if 0 <= self.index < len(obj.emitter_list):
            removed_name = obj.emitter_list[self.index].particle_name
            obj.emitter_list.remove(self.index)
            self.report({'INFO'}, f"Emitter '{removed_name}'を削除しました")
            
            # 強制的にUIを更新
            for area in context.screen.areas:
                if area.type == 'PROPERTIES':
                    area.tag_redraw()
        else:
            self.report({'ERROR'}, f"無効なインデックスです: {self.index}")
        
        return {'FINISHED'}