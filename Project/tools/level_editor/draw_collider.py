import gpu
import gpu_extras.batch
import bpy
import copy
import mathutils

class DrawCollider:
    #描画ハンドル
    handle =None

    #3Dビューに登録する描画関数
    def draw_collider():

        #頂点データ
        vertices={"pos":[]}
        #インデックスデータ
        indices=[]
        #各頂点の、オブジェクト中心からのオフセット
        offsets=[
                [-0.5,-0.5,-0.5],
                [+0.5,-0.5,-0.5],
                [-0.5,+0.5,-0.5],
                [+0.5,+0.5,-0.5],
                [-0.5,-0.5,+0.5],
                [+0.5,-0.5,+0.5],
                [-0.5,+0.5,+0.5],
                [+0.5,+0.5,+0.5],
        ]

        #立方体のX,Y,Z方向サイズ
        size=[2,2,2]

        #現在シーンのオブジェクトリストを走査
        for object in bpy.context.scene.objects:

            #コライダープロパティがなければ、描画をスキップ
            if not "collider" in object:
                continue

            center = mathutils.Vector((0,0,0))
            size=mathutils.Vector((2,2,2))

            #プロパティから値を取得
            center[0]=object["collider_center"][0]
            center[1]=object["collider_center"][1]
            center[2]=object["collider_center"][2]
            size[0]=object["collider_size"][0]
            size[1]=object["collider_size"][1]
            size[2]=object["collider_size"][2]


            #追加前の頂点数
            start = len(vertices["pos"])

            #Boxの8頂点分回す
            for offset in offsets:
                #オブジェクトの中心座標をコピー
                pos=copy.copy(center)
                #中心点を基準に各頂点ごとにずらす
                pos[0]+=offset[0]*size[0]
                pos[1]+=offset[1]*size[1]
                pos[2]+=offset[2]*size[2]
                #ローカル座標からワールド座標に変換
                pos=object.matrix_world @ pos
                #頂点データリストに座標を登録
                vertices['pos'].append(pos)

                #前面を更生する編の頂点インデックス
                indices.append([start+0,start+1])
                indices.append([start+2,start+3])
                indices.append([start+0,start+2])
                indices.append([start+1,start+3])
                #奥面を更構成する編の頂点インデックス
                indices.append([start+4,start+5])
                indices.append([start+6,start+7])
                indices.append([start+4,start+6])
                indices.append([start+5,start+7])
                #前と頂点を繋ぐ辺の頂点インデックス
                indices.append([start+0,start+4])
                indices.append([start+1,start+5])
                indices.append([start+2,start+6])
                indices.append([start+3,start+7])


        #ビルドインのシェーダーを取得
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")


        #バッチを作成
        batch=gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices=indices)

        #シェーダーのパラメータ設定
        color=[0.5,1.0,1.0,1.0]
        shader.bind()
        shader.uniform_float("color",color)
        #描画
        batch.draw(shader)
