import bpy

# ============================
# Emitterデータパネル
# ============================
class OBJECT_PT_emitter_props(bpy.types.Panel):
    bl_label = "Emitter設定"
    bl_idname = "OBJECT_PT_emitter_props"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "object"

    @classmethod
    def poll(cls, context):
        return context.object is not None

    def draw(self, context):
        layout = self.layout
        obj = context.object

        # emitter_listプロパティが存在しない場合の処理
        if not hasattr(obj, 'emitter_list'):
            layout.label(text="Emitterプロパティが見つかりません")
            layout.label(text="アドオンを無効化→有効化してください")
            return

        # 追加・削除ボタン
        row = layout.row(align=True)
        row.operator("object.add_emitter", text="Emitter追加", icon='ADD')

        # Emitterリストの表示
        if len(obj.emitter_list) == 0:
            layout.label(text="Emitterが設定されていません")
        else:
            layout.label(text=f"Emitter数: {len(obj.emitter_list)}")
            
            for i, emitter in enumerate(obj.emitter_list):
                box = layout.box()
                
                # ヘッダー行
                header = box.row()
                header.label(text=f"Emitter {i+1}", icon='PARTICLES')
                
                # 削除ボタン - プロパティを文字列として設定
                props = header.operator("object.remove_emitter", text="", icon='X')
                props.index = i
                
                # プロパティ（編集可能）
                col = box.column(align=True)
                col.prop(emitter, "particle_name", text="名前")
                col.prop(emitter, "primitive_type", text="種類")
                
            
       