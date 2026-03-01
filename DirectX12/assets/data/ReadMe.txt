敵を追加した際に編集する箇所
MapManager.hのCharacterType
AssetManager.hのMeshName
敵データ、メッシュデータのjson

敵の名前、メッシュの名前はすべて大文字


敵データjsonの構造

基本構造
敵の名前　： {
    hp
    power
    defense
    movePattern
}

movePattern : 敵の行動パターン
    Random  ランダム
    AsterA  *アルゴリズムでプレイヤーを追跡
    Sense   基本ランダム、一定範囲に入ったらA*で追跡
        senseRange　感知範囲