# memo2
- とりあえずロック機構はあきらめる？
- 出力配列が2つ以上のとき、入力配列に inplace がある場合
-- 第1番目の出力配列が inplace 配列とする

-------

# na_ndloop_main
## ndloop_cast_args
- 入力配列をキャスト
  - 型の比較
  - (ループタイプ)

## ndloop_alloc
- max_nd を得る
- lp イテレータをアロケート

- (入力配列のアクセスタイプを得る)
- (ループ関数のループタイプを得る)
- (次の条件のとき、入力配列をキャスト・コピーする
  - 配列データタイプが、ループ関数指定のデータ型とマッチしないとき。
  - ループ関数が HAS_LOOP であり、配列アクセスタイプをサポートしないとき。
  - ループ関数が NO_LOOP で、user_ndim が1以上であり、配列アクセスタイプをサポートしないとき。
- )

# ndloop_run
## ndloop_init_args

### (ndloop_set_input_iter)
- 各入力配列について
  - LITER.ptr をセット
  - 各次元について
    - LITER pos step idx をセット

### (ndloop_set_output_iter)
- 各出力配列について
  - ndloop_get_arg_type 配列dataタイプを得る
  - ndloop_set_narray_result (prepare_output_narray)
    - 入力配列に inplace またはコピー配列がある場合：
      - データタイプ、(アクセスタイプ)、shape が合い、ループ関数がinplaceサポートならば、出力配列として用いる
    - ない場合：
      - inplace がなければ、出力配列を作る
  - LITER にセット

### (ndloop_init_output)
- 初期化データがある場合
  - na_store

## ndfunc_set_user_loop
### ndfunc_check_user_loop
- ループ関数内でループするかどうかの検出
- user.ndim の調整

## loop_narray
演算　operate_narray

## (inplaceに書き戻し！)
ndloop_return_output
条件： 入力配列に inplace があり、shape が合うのに、出力配列にセットされなかったとき

- 入力配列が inplace でなく、キャストして inplace になり、出力配列にセットされたものは
  inplace フラグをはずす。
