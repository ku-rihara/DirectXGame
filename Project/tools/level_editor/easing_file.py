import bpy

from .easing_step import MYADDON_OT_add_easing_step, MYADDON_OT_remove_easing_step
from .easing_group import MYADDON_OT_add_easing_group, MYADDON_OT_remove_easing_group

class OBJECT_PT_easing_filename(bpy.types.Panel):
    """オブジェクトのイージング管理パネル"""
    bl_idname = "object_easing_filename"
    bl_label = "Easing Manager"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        layout = self.layout
        obj = context.object

        # イージンググループ管理
        group_box = layout.box()
        group_box.label(text="Easing Groups:")

        # グループ管理ボタン
        row = group_box.row()
        row.operator(MYADDON_OT_add_easing_group.bl_idname, icon='ADD')
        row.operator(MYADDON_OT_remove_easing_group.bl_idname, icon='REMOVE')

        # 現在のグループ数を表示
        if "easing_group_count" in obj:
            group_count = obj["easing_group_count"]
            group_box.label(text=f"Total Groups: {group_count}")

            # 各グループの設定を表示
            for group in range(group_count):
                self.draw_group(group_box, obj, group)
        else:
            group_box.label(text="No groups created yet")

    def draw_group(self, layout, obj, group_id):
        """個別のイージンググループを描画"""
        group_box = layout.box()
        group_box.label(text=f"Group {group_id}:")

        # 開始時間設定を追加
        start_time_prop = f"easing_group_{group_id}_start_time"
        
        # プロパティが存在するかチェックして表示
        row = group_box.row()
        row.label(text="Start Time:")
        if start_time_prop in obj:
            row.prop(obj, f'["{start_time_prop}"]', text="")
        else:
            # プロパティが存在しない場合は警告を表示
            row.label(text="Not initialized", icon='ERROR')

        # ステップ管理ボタン
        row = group_box.row()
        add_op = row.operator(MYADDON_OT_add_easing_step.bl_idname, icon='ADD')
        add_op.group_id = group_id
        remove_op = row.operator(MYADDON_OT_remove_easing_step.bl_idname, icon='REMOVE')
        remove_op.group_id = group_id

        # 現在のステップ数を表示
        step_count_prop = f"easing_group_{group_id}_step_count"
        if step_count_prop in obj:
            step_count = obj[step_count_prop]
            group_box.label(text=f"Steps: {step_count}")

            # SRT固定ラベル
            srt_labels = ["Scale", "Rotation", "Transform"]

            # 各ステップの設定を表示
            for step in range(1, step_count + 1):
                step_box = group_box.box()
                step_box.label(text=f"Step {step}:")

                # 各ファイル設定（SRT固定）
                for i in range(1, 4):
                    file_prop = f"easing_group_{group_id}_step_{step}_file{i}"

                    if file_prop in obj:
                        row = step_box.row()

                        # SRTタイプをラベルとして表示
                        row.label(text=srt_labels[i-1] + ":")

                        # ファイル名入力（テキストボックス）
                        row.prop(obj, f'["{file_prop}"]', text="")
        else:
            group_box.label(text="No steps in this group")