import bpy

from .enemy_spawn_manager import SpawnGroupManager 

class MYADDON_PT_spawn_manager_panel(bpy.types.Panel):
    bl_idname = "MYADDON_PT_spawn_manager_panel"
    bl_label = "スポーン管理ツール"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'Spawn Manager'
    
    def draw(self, context):
        layout = self.layout
        
        # === スポーンポイント作成セクション ===
        box = layout.box()
        box.label(text="スポーンポイント作成", icon='ADD')
        
        # 通常敵と強敵の作成ボタン
        row = box.row(align=True)
        row.operator("myaddon.create_spawn_normalenemy", text="通常敵", icon='CUBE')
        row.operator("myaddon.create_spawn_strongenemy", text="強敵", icon='META_CUBE')
        
        # シンボルインポートボタン
        box.operator("myaddon.spawn_import_symbol", text="シンボルをインポート", icon='IMPORT')
        
        # === グループ表示切り替えセクション ===
        box = layout.box()
        box.label(text="グループ表示切り替え", icon='HIDE_OFF')
        
        groups = SpawnGroupManager.get_existing_groups()
        if groups:
            # グループを2列で表示
            for i in range(0, len(groups), 2):
                row = box.row(align=True)
                
                # 左側のグループ
                group_id = groups[i]
                objects = SpawnGroupManager.get_objects_in_group(group_id)
                visible_count = sum(1 for obj in objects if not obj.hide_viewport)
                total_count = len(objects)
                is_visible = visible_count > 0
                
                icon = 'HIDE_OFF' if is_visible else 'HIDE_ON'
                op = row.operator("myaddon.toggle_group_visibility", 
                                text=f"G{group_id} ({visible_count}/{total_count})", 
                                icon=icon)
                op.group_id = group_id
                
                # 右側のグループ（存在する場合）
                if i + 1 < len(groups):
                    group_id = groups[i + 1]
                    objects = SpawnGroupManager.get_objects_in_group(group_id)
                    visible_count = sum(1 for obj in objects if not obj.hide_viewport)
                    total_count = len(objects)
                    is_visible = visible_count > 0
                    
                    icon = 'HIDE_OFF' if is_visible else 'HIDE_ON'
                    op = row.operator("myaddon.toggle_group_visibility", 
                                    text=f"G{group_id} ({visible_count}/{total_count})", 
                                    icon=icon)
                    op.group_id = group_id
        else:
            box.label(text="グループが見つかりません", icon='ERROR')
        
        # 全体操作
        row = box.row(align=True)
        row.operator("myaddon.show_all_groups", text="全て表示", icon='HIDE_OFF')
        row.operator("myaddon.hide_all_groups", text="全て非表示", icon='HIDE_ON')

    
        # === グループタイム設定セクション ===
        box = layout.box()
        box.label(text="グループタイム設定", icon='TIME')
        
        if groups:
            # 各グループのタイム設定を表示
            for group_id in groups:
           
                row = box.row(align=True)
                row.label(text=f"G{group_id}:", icon='OUTLINER_OB_GROUP_INSTANCE')
                
                # 現在の設定時間を表示
                current_time = SpawnGroupManager.get_group_spawn_time(group_id)
                row.label(text=f"{current_time:.1f}s")
                
                # 減少ボタン（大きく）
                op = row.operator("myaddon.adjust_spawn_time", text="-1.0")
                op.group_id = group_id
                op.delta = -1.0
                
                # 減少ボタン（小さく）
                op = row.operator("myaddon.adjust_spawn_time", text="-0.1")
                op.group_id = group_id
                op.delta = -0.1
                
                # 増加ボタン（小さく）
                op = row.operator("myaddon.adjust_spawn_time", text="+0.1")
                op.group_id = group_id
                op.delta = 0.1
                
                # 増加ボタン（大きく）
                op = row.operator("myaddon.adjust_spawn_time", text="+1.0")
                op.group_id = group_id
                op.delta = 1.0
                
        
        # === ツールセクション ===
        box = layout.box()
        box.label(text="ツール", icon='TOOL_SETTINGS')
        
        # 統計情報の表示
        total_spawns = len([obj for obj in bpy.data.objects if "spawn_group" in obj])
        box.label(text=f"総スポーンポイント数: {total_spawns}")
        
        if groups:
            box.label(text=f"アクティブグループ数: {len(groups)}")

        box = layout.box()
        box.label(text="エクスポート", icon='EXPORT')
        box.operator("myaddon.export_spawn_data", text="スポーンデータエクスポート", icon='EXPORT')


# 追加のオペレーター
class MYADDON_OT_show_all_groups(bpy.types.Operator):
    bl_idname = "myaddon.show_all_groups"
    bl_label = "全グループ表示"
    bl_description = "すべてのスポーングループを表示します"
    
    def execute(self, context):
        groups = SpawnGroupManager.get_existing_groups()
        for group_id in groups:
            SpawnGroupManager.set_group_visibility(group_id, True)
        
        self.report({'INFO'}, f"{len(groups)} 個のグループを表示しました")
        return {'FINISHED'}

class MYADDON_OT_hide_all_groups(bpy.types.Operator):
    bl_idname = "myaddon.hide_all_groups"
    bl_label = "全グループ非表示"
    bl_description = "すべてのスポーングループを非表示にします"
    
    def execute(self, context):
        groups = SpawnGroupManager.get_existing_groups()
        for group_id in groups:
            SpawnGroupManager.set_group_visibility(group_id, False)
        
        self.report({'INFO'}, f"{len(groups)} 個のグループを非表示にしました")
        return {'FINISHED'}