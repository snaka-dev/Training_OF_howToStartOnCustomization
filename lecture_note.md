# OpenFOAMカスタマイズの始め方 (v1912)
##### 2020年8月23日オープンCAE勉強会＠富山 (富山県立大学　中川慎二)

## Disclaimer

   OPENFOAM® is a registered trade mark of OpenCFD Limited, the producer of the OpenFOAM software and owner of the OPENFOAM® and OpenCFD® trade marks. This offering is not approved or endorsed by OpenCFD Limited.

## 注意

　本資料の内容は，OpenFOAMユーザーガイド，プログラマーズガイド，OpenFOAM Wiki，CFD Online，その他多くの情報を参考にしています。開発者，情報発信者の皆様に深い謝意を表します。

　内容は，講師の個人的な経験（主に，卒研生等とのコードリーディング）から得た知識を共有するものです。この内容の正確性を保証することはできません。この情報を使用したことによって問題が生じた場合，その責任は負いかねますので，予めご了承ください。

<a name="tableOfContents"></a>
## 目次 ##
- [はじめに・目的](#objective)
- [環境](#version)
- [想定する受講者・前提知識](#presupposition)
- [基本方針](#direction)

- [OpenFOAMカスタマイズの始め方](#openfoamカスタマイズの始め方)
        - [2020年8月23日オープンCAE勉強会＠富山 (富山県立大学　中川慎二)](#2020年8月23日オープンcae勉強会富山-富山県立大学中川慎二)
  - [Disclaimer](#disclaimer)
  - [注意](#注意)
  - [目次](#目次)
  - [はじめに・目的](#はじめに目的)
  - [環境](#環境)
  - [想定する受講者・前提知識](#想定する受講者前提知識)
  - [基本方針](#基本方針)
  - [準備](#準備)
    - [本書の標記について](#本書の標記について)
    - [Linuxコマンドの確認](#linuxコマンドの確認)
      - [OpenFOAM Linux Guide](#openfoam-linux-guide)
      - [sed](#sed)
  - [使用環境の確認](#使用環境の確認)
  - [新しい名前のソルバ作成](#新しい名前のソルバ作成)
    - [標準ファイルのコピー (solver)](#標準ファイルのコピー-solver)
    - [名前の変更](#名前の変更)
    - [ソルバ名の変更：Make/filesの修正](#ソルバ名の変更makefilesの修正)
    - [コンパイル](#コンパイル)
  - [テスト用例題（ケース）の作成](#テスト用例題ケースの作成)
    - [標準例題のコピー copy original files (tutorial)](#標準例題のコピー-copy-original-files-tutorial)
    - [system/controlDictの編集（application名の変更）](#systemcontroldictの編集application名の変更)
    - [計算実行と結果の確認](#計算実行と結果の確認)
  - [ライブラリのカスタマイズ手順の確認（ライブラリ全体の複製）](#ライブラリのカスタマイズ手順の確認ライブラリ全体の複製)
    - [copy original files (library)](#copy-original-files-library)
    - [モデルの複製と名前の変更](#モデルの複製と名前の変更)
    - [ライブラリに新クラスを追加：Make/files](#ライブラリに新クラスを追加makefiles)
    - [コンパイル](#コンパイル-1)
    - [動作確認](#動作確認)
      - [例題の修正：改名した粘性モデルの使用](#例題の修正改名した粘性モデルの使用)
      - [考察](#考察)
      - [動作時に使用されるライブラリの確認](#動作時に使用されるライブラリの確認)
  - [ライブラリのカスタマイズ（新しい粘性モデルだけを含むライブラリの作成）](#ライブラリのカスタマイズ新しい粘性モデルだけを含むライブラリの作成)
    - [copy original files](#copy-original-files)
    - [modify source files](#modify-source-files)
    - [Make ディレクトリの files と options ファイルを修正](#make-ディレクトリの-files-と-options-ファイルを修正)
      - [Make/files](#makefiles)
      - [Make/options](#makeoptions)
    - [compile](#compile)
  - [例題の作成 create a tutarial case](#例題の作成-create-a-tutarial-case)
  - [考察](#考察-1)
    - [ライブラリ丸ごとコピーと，クラス単位の改造，どちらが良いの？](#ライブラリ丸ごとコピーとクラス単位の改造どちらが良いの)
    - [乱流モデルとtransportModelとの違い](#乱流モデルとtransportmodelとの違い)
  - [モデルのさらなる改造](#モデルのさらなる改造)
    - [newCrossPowerLaw.H の修正](#newcrosspowerlawh-の修正)
    - [newCrossPowerLaw.C の修正](#newcrosspowerlawc-の修正)
    - [コンパイル](#コンパイル-2)
    - [動作確認](#動作確認-1)
- [サンプルファイル](#サンプルファイル)
  - [参考情報](#参考情報)
    - [ソルバからの書き出しにfunctionObjectが使えるのか？](#ソルバからの書き出しにfunctionobjectが使えるのか)
- [作業メモ](#作業メモ)
  - [環境変数](#環境変数)


<a name="objective"></a>
## はじめに・目的

OpenFOAMをはじめてカスタマイズする初心者を想定し，基礎の基礎をハンズオン形式で学びます。OpenFOAMの改造をどこからはじめて良いか分からないといった人が，元のコードをコピーして，新たな名前のモデルやライブラリを作る手順を確認していきます。

[参考サイト1：オープンCAE勉強会＠富山（第34回2015年7月25日）「OpenFOAM 非ニュートンモデル カスタマイズ（秋山氏）」](http://eddy.pu-toyama.ac.jp/%E3%82%AA%E3%83%BC%E3%83%97%E3%83%B3CAE%E5%8B%89%E5%BC%B7%E4%BC%9A-%E5%AF%8C%E5%B1%B1/?action=cabinet_action_main_download&block_id=99&room_id=1&cabinet_id=1&file_id=125&upload_id=255)

[参考サイト２：上記の補足説明とコード](https://github.com/snaka-dev/nonNewtonianCustomizeTest01)

上記参考サイトの情報と，本演習との違いを考えることで，同じ結果を得るための複数のカスタマイズ方法を知ることができます。

下記の内容の一部や，ライブラリによる違いについても説明する予定です。
[参考サイト３](https://qiita.com/snaka-dev/items/43779235a4ed966788aa)

<a name="version"></a>
## 環境 ##

この資料は，OpenFOAM v1912 を基準として作成した。


<a name="presupposition"></a>
## 想定する受講者・前提知識 ##

OpenFOAM初心者であり，ソースコードのカスタマイズを目指す人を対象とします。［想定する受講者：機械系の大学４年生で，卒業研究にOpenFOAMを使い，少しプログラムを変更する必要がある。知識は豊富ではないが，やる気は満々である。自学自習を厭わない。］

次のような知識・経験を前提とします。リンク先などを事前にお読みください。

- OpenFOAMのごく基本的な事を知っている。（例題の1つや2つを実行したことがある。）

- 何らかのプログラミング言語を学習したことがあり，プログラミングに関する基本的な知識がある。（変数，関数，型，などの基礎知識）

- Linuxの端末上で，ごく基本的な操作（ファイルのコピーや移動）ができる。（テキストに書いてあることをタイプして実行できる。）
    - 参考になるサイト：OpenFOAM Linux Guide   https://cfd.direct/openfoam/linux-guide/

- Linux，仮想マシンの基礎的な使い方（[オープンCAE勉強会＠富山第80回の講習](https://opencae-toyama.connpass.com/event/172787/)の事前準備動画を閲覧した）

- [オープンCAE勉強会＠富山第80回の講習](https://opencae-toyama.connpass.com/event/172787/)の事前準備を実施した，または，その程度の作業は軽くこなせる

- OpenFOAMのコンパイルに関するユーザーガイドを一読したことがある。
    - [OpenFOAM v8 User Guide: 3.2 Compiling applications & libraries へのリンク](https://cfd.direct/openfoam/user-guide/v8-compiling-applications/#x10-710003.2)

<a name="direction"></a>
## 基本方針

改造は少しずつ。改造・コンパイル・テストを細かくくり返す。

この資料では，実施する作業の説明につづき，それを実現するためのコマンドを記載している。GUI（ファイルマネージャーやテキストエディタ）での操作を希望する人は，説明に基づいて作業してください。

一部，ファイルの内容を修正する作業については，変更内容だけを記載する部分がある。その場合には，自身の作業しやすい方法で，内容を変更してください。

エラーが発生した場合には，エラーメッセージをしっかりと読む。1番はじめのエラーへ対応する。

**OpenFOAMで使用するコンパイル・スクリプト wmake については，[別の資料（OpenFOAM コンパイルに関するメモ：リンク）](https://nakagawa-lab.kibe.la/shared/entries/babfd4ea-6b57-4f0a-b218-762d5d4d8d8a)で説明します。**


|  |  |
| -- | -- |
| 1 | 名前を変更したソルバの作成<br> 　　simpleFoam から mySimpleFoam へ |
| 2 | テスト例題の作成 |
| 3 | 名前を変更したライブラリと物理モデルの作成(全コピ法)<br> 　　transportModels から myCrossPowerLaw へ |
| 4 | ライブラリのカスタマイズ（新しい粘性モデルだけを含むライブラリの作成）<br> 　　newCrossPowerLawクラスと newCrossPowerLawライブラリ |
| 5 | 例題の作成 |

<a name="pre"></a>
## 準備

<a name="remarks"></a>
### 本書の標記について

端末で実行するコマンド

> echo $FOAM_SRC

プログラムのソースコード

```
int n;
```

<a name="linuxCommand"></a>
### Linuxコマンドの確認

端末内での実行場所移動：cd （チェンジ ディレクトリ）

> cd   _移動先_

ディレクトリの作成：mkdir （メーク ディレクトリ）

> mkdir   _ディレクトリ名_

　オプション　-p   親ディレクトリも同時に作成

ファイルやディレクトリのコピー：cp （コピー）

> cp   _元ファイル_   _コピー先_

　オプション　-p   元のファイル属性を保持（preserve）

　オプション　-r   ディレクトリの中身もコピー ← 再帰的にコピー（recursive）

ファイルやディレクトリの移動：mv （ムーブ）

> mv   _移動元_   _移動先_

　この mv コマンドは，名前の変更にも使う。

#### OpenFOAM Linux Guide

とても，とても，参考になるサイト。OpenFOAMをLinux上で使用する初心者は，一読すべきである。

- 参考になるサイト：OpenFOAM Linux Guide   https://cfd.direct/openfoam/linux-guide/

#### sed 
sed はストリームエディタである。この講習では，テキストファイルを変更するために使用している。

使い方の参考になるサイト： https://linuxjm.osdn.jp/html/GNU_sed/man1/sed.1.html

[［手順一覧に戻る］](#tableOfContents)

<a name="checkEnvVariables"></a>
## 使用環境の確認

使用するマシンのOpenFOAMの環境を確認する。わかりやすい説明が [OpenFOAM v8 User Guide: 3.2.2.5 wmake environment variables](https://cfd.direct/openfoam/user-guide/v8-compiling-applications/#x10-780003.2.2.5) にある。

次の表に，主な環境変数名と意味を示す。

| 環境変数　システム     | 環境変数　ユーザー                     | 意味 |
| ------------------- | ----------------------------------- | ------ |
| WM_PROJECT_DIR | WM_PROJECT_USER_DIR | OpenFOAM関連ファイル格納場所 |
| FOAM_SRC       |   `$WM_PROJECT_USER_DIR/src/`   | OpenFOAMソースコード（ライブラリ）格納場所  |
| FOAM_SOLVERS   |  `$WM_PROJECT_USER_DIR/solvers/`  | ソルバ（実行ファイル）の格納場所 |
| FOAM_LIBBIN    | FOAM_USER_LIBBIN | ライブラリ（実行ファイル）の格納場所 |
| FOAM_APPBIN    | FOAM_USER_APPBIN | ソルバ（実行ファイル）の格納場所   |
| FOAM_TUTORIALS    | FOAM_RUN | 例題の格納場所   |


OpenFOAMが実行できる端末において、次のコマンドを実行して、その結果の出力を確認する。`echo` は，その後の内容を画面に表示するlinuxのコマンドである。変数名の内容を表すために、変数名の前に 記号`$` をつける。

> echo $WM_PROJECT_DIR 

ここで，$WM_PROJECT_DIR はOpenFOAMの環境変数であり，OpenFOAMのインストールディレクトリである。

同様に，下記のコマンドを実行する。

システムのOpenFOAMソースコード格納場所：$FOAM_SRC
> echo $FOAM_SRC

ユーザーのOpenFOAM関連ファイル格納場所：$WM_PROJECT_USER_DIR
> echo $WM_PROJECT_USER_DIR

ユーザーがカスタマイズしたライブラリ（実行ファイル）の格納場所：$FOAM_USER_LIBBIN
> echo $FOAM_USER_LIBBIN

ユーザーがカスタマイズしたソルバ（実行ファイル）の格納場所：$FOAM_USER_APPBIN
> echo $FOAM_USER_APPBIN

実行結果例
```bash
bash-4.2$ echo $WM_PROJECT_DIR 
/opt/OpenFOAM/OpenFOAM-v1912
bash-4.2$ echo $FOAM_SRC 
/opt/OpenFOAM/OpenFOAM-v1912/src
bash-4.2$ echo $FOAM_SOLVERS 
/opt/OpenFOAM/OpenFOAM-v1912/applications/solvers
bash-4.2$ echo $FOAM_TUTORIALS 
/opt/OpenFOAM/OpenFOAM-v1912/tutorials
bash-4.2$ 
bash-4.2$ echo $WM_PROJECT_USER_DIR 
/home/user/OpenFOAM/user-v1912
bash-4.2$ echo $FOAM_RUN 
/home/user/OpenFOAM/user-v1912/run
bash-4.2$ echo $FOAM_USER_APPBIN 
/home/user/OpenFOAM/user-v1912/platforms/linux64GccDPInt32Opt/bin
bash-4.2$ echo $FOAM_USER_LIBBIN 
/home/user/OpenFOAM/user-v1912/platforms/linux64GccDPInt32Opt/lib
```

[［手順一覧に戻る］](#tableOfContents)

<a name="createNewNameSolver"></a>
## 新しい名前のソルバ作成

ここでは，既存ソルバのコピー，名前の変更，コンパイルを実行する。内容は変更しないが，OpenFOAMの基本的なコンパイル方法、wmakeコマンドについて学ぶ。

<a name="copyStandardSolver"></a>
### 標準ファイルのコピー (solver)

|  | 値 |
| -- | -- |
| コピー元 | `$FOAM_SOLVERS/incompressible/simpleFoam/` |
| コピー先 | `$WM_PROJECT_USER_DIR/solvers/` |

ユーザーのオリジナルソルバを格納する場所として、`$WM_PROJECT_USER_DIR/solvers/` を作成する。

> mkdir $WM_PROJECT_USER_DIR/solvers/

作成したディレクトリ内に，`$FOAM_SOLVERS/incompressible/simpleFoam/` をコピーする。

> cp -r $FOAM_SOLVERS/incompressible/simpleFoam/ $WM_PROJECT_USER_DIR/solvers/simpleFoam/

コピーしたディレクトリの内部には，今回は不要な3つのディレクトリ（overSimpleFoam  porousSimpleFoam  SRFSimpleFoam）が存在する。これらを削除する。

> cd $WM_PROJECT_USER_DIR/solvers/simpleFoam

> rm -r *Foam

なお、最後のコマンドで使用している記号 `*` は、ワイルドカードとして使っており、その前にいろいろな文字列が入るものを指し示す。`rm` は，ファイルなどを削除（remove）するコマンドであり，オプション `-r` をつけることで再帰的に（ディレクトリの中身とディレクトリを）削除できる。

<a name="renameStandardSolver"></a>
### 名前の変更

ディレクトリ名，ファイル名，ファイル中のクラス名や関数名を変更する。

|  | 値 |
| -- | -- |
| 変更前 |  `simpleFoam` |
| 変更後 | `mySimpleFoam` |

ディレクトリ名を `simpleFoam` から `mySimpleFoam` に変更する。

> cd $WM_PROJECT_USER_DIR/solvers/

> mv simpleFoam mySimpleFoam

ファイル名を `simpleFoam.C` から `mySimpleFoam.C` に変更する。

> cd mySimpleFoam

> mv simpleFoam.C mySimpleFoam.C


`mySimpleFoam.C` の中に現れる  `simpleFoam` を `mySimpleFoam` に変更する。これは，プログラムの動作には全く影響ないが，ファイル冒頭にあるコメント・説明部分を修正するものである。後々のトラブルなどを防ぐために，コメント等の修正も実施することが望ましい。（さらに進んだ使い方を考えると，自動生成されるマニュアルが正確になることに繋がる。）

> sed -i s/simpleFoam/mySimpleFoam/g mySimpleFoam.C

### ソルバ名の変更：Make/filesの修正
`Make/files` の中に現れる  `simpleFoam` を `mySimpleFoam` に変更する。この作業によって，実行時のソルバ名が mySimpleFoam に変わる。

> sed -i s/simpleFoam/mySimpleFoam/g Make/files

さらに，`Make/files` の中で，コンパイル後のファイル保存先を  `FOAM_APPBIN` から `FOAM_USER_APPBIN` に変更する。

> sed -i s/FOAM_APPBIN/FOAM_USER_APPBIN/g Make/files

**これらファイルの記述内容の意味については，wmake については，別の資料で説明します。[リンク](https://nakagawa-lab.kibe.la/shared/entries/babfd4ea-6b57-4f0a-b218-762d5d4d8d8a)**

<a name="compileRenamedSolver"></a>
### コンパイル

正しく変更できたかを確認するため，コンパイルする。

端末上で `$WM_PROJECT_USER_DIR/solvers/mySimpleFoam` に移動する。

> cd $WM_PROJECT_USER_DIR/solvers/mySimpleFoam

`wmake` コマンドを実行する前に，過去のコンパイルの情報などを削除する `wclean` スクリプトを実行しておく。

> wclean

`wmake` コマンドを実行してコンパイルする。`wmake` は，OpenFOAMに付属するスクリプトであり，一般的にコンパイルに使われる `make` コマンドにOpenFOAMに必要な設定などが追加されている。

> wmake

上記コマンドを実行し，端末に表示されるメッセージにエラーがないことを確認する。

さらに，ディレクトリ  `$FOAM_USER_APPBIN` (これは `$WM_PROJECT_USER_DIR/platforms/linux64GccDPInt32Opt/bin/`と同じ。コンパイルオプションによって異なる場合がある。) に ファイル mySimpleFoam が作成されていることを確認する。

> cd $FOAM_USER_APPBIN

> ls -al

[［手順一覧に戻る］](#tableOfContents)

<a name="createTestCase"></a>
## テスト用例題（ケース）の作成

<a name="copyStandardTutorial"></a>
### 標準例題のコピー copy original files (tutorial)

標準例題を複製し，作成したソルバが動作することを確認するための例題を作成する。

|    | 値 |
| -- | -- |
| コピー元 | `$FOAM_TUTORIALS/incompressible/simpleFoam/simpleCar/` |
| コピー先 | `$FOAM_RUN` |

> mkdir -p $FOAM_RUN

> cp -r $FOAM_TUTORIALS/incompressible/simpleFoam/simpleCar/ $FOAM_RUN

<a name="modifyControlDict"></a>
### system/controlDictの編集（application名の変更）

作業先の `simpleCar` ディレクトリに移動する。`system/controlDict`ファイル内の`simpleFoam` を `mySimpleFoam` に変更する。これは，Allrunスクリプトなどで計算実行する際に，controlDictファイルからソルバ名を読み込むためである。

> cd $FOAM_RUN/simpleCar

> sed -i s/simpleFoam/mySimpleFoam/g system/controlDict

<a name="runAndCheck"></a>
### 計算実行と結果の確認

過去の実行結果を削除し，改めて，Allrunスクリプトにより計算を実行する。

> foamCleanTutorials

> ./Allrun

ケースディレクトリに `log.mySimpleFoam` が作成されていることを確認する。そのファイルを開き，`mySimpleFoam` が実行されていることを確認する。（ファイル冒頭部分の `Exec:` 欄にソルバ名が記録されている。）

計算が完了するまでは時間がかかる。途中で停止したい場合には，計算を実行した端末上で `Ctrl + C` キーを入力する。

[［手順一覧に戻る］](#tableOfContents)

<a name="howtoCompileLib"></a>
## ライブラリのカスタマイズ手順の確認（ライブラリ全体の複製）

ここでは，OpenFOAMの標準ライブラリ `transportModels` ディレクトリ全体をコピーし，その中で改造が必要な部分だけを変更する方法について説明する。

大まかな作業手順は，既存の全ライブラリをコピー，改造部分の名前の変更，コンパイルとなる。

<a name="copyStandardLib"></a>
### copy original files (library)

|    | 値 |
| -- | -- |
| コピー元 | `$FOAM_SRC`/transportModels |
| コピー先 |  `$WM_PROJECT_USER_DIR/src/transportModels` |

OpenFOAMのsrc/transportModelsディレクトリ全体を，ユーザのディレクトリにコピーする。**ファイル数が多いので，少し時間がかかる。**

> mkdir $WM_PROJECT_USER_DIR/src/

> cp -r $FOAM_SRC/transportModels/  $WM_PROJECT_USER_DIR/src/

<a name="copyAndRenameStandardClass"></a>
### モデルの複製と名前の変更

OpenFOAMに標準で作成されているCrossPowerLaw（非ニュートン粘性モデル）から，myCrossPowerLawモデルを作成する。ファイル名，ファイル中のクラス名や関数名を変更する。

ディレクトリ `CrossPowerLaw` を `myCrossPowerLaw` という名前にして複製する。

> cp -r $WM_PROJECT_USER_DIR/src/transportModels/incompressible/viscosityModels/CrossPowerLaw/  $WM_PROJECT_USER_DIR/src/transportModels/incompressible/viscosityModels/myCrossPowerLaw/

新しく作成された myCrossPowerLaw ディレクトリへ移動する。そこに格納されている2つのファイル名を，CrossPowerLawからmyCrossPowerLawへ変更する。

> cd $WM_PROJECT_USER_DIR/src/transportModels/incompressible/viscosityModels/myCrossPowerLaw

> mv CrossPowerLaw.C myCrossPowerLaw.C

> mv CrossPowerLaw.H myCrossPowerLaw.H

上記の作業で作成された myCrossPowerLaw.H および myCrossPowerLaw.C ファイルの中で，CrossPowerLaw とかかれた部分を，myCrossPowerLaw に書き換える。（ファイルを開いて，エディタの検索と置換機能を使う。あるいは，下記コマンドを実行する。）

> sed -i s/CrossPowerLaw/myCrossPowerLaw/g myCrossPowerLaw.H

> sed -i s/CrossPowerLaw/myCrossPowerLaw/g myCrossPowerLaw.C

<a name="addNewClassToLib"></a>
### ライブラリに新クラスを追加：Make/files

myCrossPowerLaw関連ファイルがコンパイルされるように，`$WM_PROJECT_USER_DIR/src/transportModels/incompressible/Make` の中にあるfiles ファイルを修正する。 

5行目をコピーし，6行目に貼り付けて `viscosityModels/myCrossPowerLaw/myCrossPowerLaw.C` に修正する。

> cd $WM_PROJECT_USER_DIR/src/transportModels/incompressible/Make

> sed -i -e '/CrossPowerLaw.C$/a viscosityModels\/myCrossPowerLaw\/myCrossPowerLaw.C' files

さらに，コンパイル後の保存先を `FOAM_LIBBIN` から `FOAM_USER_LIBBIN` に変更する。

> sed -i s/FOAM_LIBBIN/FOAM_USER_LIBBIN/g files

<a name="compileRenamedLib"></a>
### コンパイル

ディレクトリ `$WM_PROJECT_USER_DIR/src/transportModels/incompressible` から，wmake コマンドを実行して，ソースコードをコンパイルする。

> cd $WM_PROJECT_USER_DIR/src/transportModels/incompressible

> wclean

> wmake

コンパイルに成功すると，`$FOAM_USER_LIBBIN`に `libincompressibleTransportModels.so` というファイルが生成される。［ 講習会用マシンでは， `/home/user/OpenFOAM/user-v1912/platforms/linux64GccDPInt32Opt/lib/libincompressibleTransportModels.so` となる。］

<a name="testRenamedLib"></a>
### 動作確認

#### 例題の修正：改名した粘性モデルの使用

先ほど作成した例題 `$FOAM_RUN/simpleCar` を使って，作成したモデルの実行を確認する。

例題で使用する粘性モデルをmyCrossPowerLawに変更する。`constant/transportProperties` 
ファイルを開き，transportModel に myCrossPowerLaw を指定する。myCrossPowerLawCoeffsを次のように指定する。（`$FOAM_TUTORIALS/incompressible/nonNewtonianIcoFoam/offsetCylinder/constant/transportProperties` を参考にしてもよい。）

```
transportModel  myCrossPowerLaw;
myCrossPowerLawCoeffs
{
    nu0         0.01;
    nuInf       10;
    m           0.4;
    n           3;
}
```

例題を実行し，ログを確認して，問題ないことを確認する。ログの冒頭付近（39行目）に，下記の記述があり，作成したモデルが使用されていることを確認する。

`Selecting incompressible transport model myCrossPowerLaw`

#### 考察

これまでの作業によって、同じ名前 `incompressibleTransportModels` のライブラリ（ファイル名は `libincompressibleTransportModels.so`）が2か所に作成されたことになる。

```shell
bash-4.2$ ls -al $FOAM_LIBBIN/libincompressibleTransportModels.so 
-rwxr-xr-x 1 1027 root 763016 Dec 23  2019 /opt/OpenFOAM/OpenFOAM-v1912/platforms/linux64GccDPInt32Opt/lib/libincompressibleTransportModels.so
bash-4.2$ ls -al $FOAM_USER_LIBBIN/libincompressibleTransportModels.so 
-rwxr-xr-x 1 user user 788256 Aug 20 00:46 /home/user/OpenFOAM/user-v1912/platforms/linux64GccDPInt32Opt/lib/libincompressibleTransportModels.so
```

同じ名前のライブラリが2カ所あるが，どうして，新たに作成したライブラリ（myCrossPowerLawはこちらにしか存在しない）が使用されたのであろう？

OpenFOAM では、`$FOAM_USER_LIBBIN` に存在するライブラリを優先して使用する。

同名のライブラリが存在する時や，一部をカスタマイズしたライブラリを作成した場合に，使用者が予期するのとは異なる方のライブラリが使用されているという恐れもある。注意が必要である。


#### 動作時に使用されるライブラリの確認

>  ldd $FOAM_APPBIN/simpleFoam

上記コマンドの実行結果の一部を示す。incompressibleTransportModels は$FOAM_USER_LIBBIN のものが使用されることが確認できる。（incompressibleTurbulenceModelsライブラリは、$FOAM_USER_LIBBIN のものが使われている。）

```console
libincompressibleTurbulenceModels.so => /home/user/OpenFOAM/OpenFOAM-v2006/platforms/linux64GccDPInt32Opt/lib/libincompressibleTurbulenceModels.so (0x00007f32fbd7d000)

libincompressibleTransportModels.so => /home/user/OpenFOAM/user-v2006/platforms/linux64GccDPInt32Opt/lib/libincompressibleTransportModels.so (0x00007f32fbca4000)

```

[［手順一覧に戻る］](#tableOfContents)


<a name="createNewLib"></a>
## ライブラリのカスタマイズ（新しい粘性モデルだけを含むライブラリの作成）

先にコピーした transportModelsディレクトリには，多くのファイルが存在する。大部分は元のままである。

今回は，必要なファイルだけを複製してカスタマイズする方法を考える。

|    | 値 |
| -- | -- |
| コピー元 | `$FOAM_SRC/transportModels/incompressible/viscosityModels/CrossPowerLaw/` |
| コピー先 | `$WM_PROJECT_USER_DIR/src/newCrossPowerLaw/` |

<a name="copyStandLib02"></a>
### copy original files

OpenFOAM標準のCrossPowerLawモデルのコード**だけ**を，`$WM_PROJECT_USER_DIR/src` ディレクトリにコピーする。

> cp -r $FOAM_SRC/transportModels/incompressible/viscosityModels/CrossPowerLaw/  $WM_PROJECT_USER_DIR/src/newCrossPowerLaw/

コピーしたファイルの名前を，CrossPowerLawからnewCrossPowerLawに変更する。CrossPowerLaw.depファイルが存在する場合には削除する。

> cd $WM_PROJECT_USER_DIR/src/newCrossPowerLaw/

> mv CrossPowerLaw.H newCrossPowerLaw.H

> mv CrossPowerLaw.C newCrossPowerLaw.C

同じディレクトリ内に，コンパイルに必要な情報をコピーする。非ニュートン粘性モデルライブラリのためのMakeディレクトリ（`$FOAM_SRC/transportModels/incompressible/Make/`）を，`$WM_PROJECT_USER_DIR/src/newCrossPowerLaw/` にコピーする。

> cp -R $FOAM_SRC/transportModels/incompressible/Make/  $WM_PROJECT_USER_DIR/src/newCrossPowerLaw/

<a name="modifyLib"></a>
### modify source files

ファイル内のモデル名をnewCrossPowerLawに変更する。

> sed -i s/CrossPowerLaw/newCrossPowerLaw/g newCrossPowerLaw.H

> sed -i s/CrossPowerLaw/newCrossPowerLaw/g newCrossPowerLaw.C

<a name="modifyMakeFiles"></a>
### Make ディレクトリの files と options ファイルを修正

新たなモデルを独立したライブラリとしてコンパイルするために，コンパイルに必要な情報を修正する。この情報は，`$WM_PROJECT_USER_DIR/src/newCrossPowerLaw/Make` ディレクトリ内に格納されている。

> cd $WM_PROJECT_USER_DIR/src/newCrossPowerLaw/Make

#### Make/files

新しく作成したソースコード newCrossPowerLaw.C だけをコンパイルし，libnewCrossPowerLawという名前のライブラリを作成する。filesファイルの内容を下記に書き換える。

```C++
newCrossPowerLaw.C

LIB = $(FOAM_USER_LIBBIN)/libnewCrossPowerLaw
```

#### Make/options

新しく作成したソースコード newCrossPowerLaw.C のコンパイルに必要なインクルードファイルの場所を指定する。ここでは，インクルードするファイルを，newCrossPowerLawクラスに必要なものだけにした。

Make/options ファイルを，下記のとおりに書き換える。

```
EXE_INC = \
    -I$(LIB_SRC)/transportModels/incompressible/lnInclude \
    -I$(LIB_SRC)/finiteVolume/lnInclude
LIB_LIBS = \
    -lfiniteVolume
```

ここで，`$(LIB_SRC)/transportModels/incompressible/lnInclude` を指定しているのは，viscosityModel.H ファイルをインクルードするためである。

newCrossPowerLawのソースコードでインクルードされる他のファイルは，Make/optionsで明示的に指定した finiteVolume または 自動的に指定されている OpenFOAM ライブラリに含まれている。


<a name="compileModifiedLib"></a>
### compile

> cd $WM_PROJECT_USER_DIR/src/newCrossPowerLaw

> wmake

ライブラリなので，wmake libso としてもよい。wmake を実行するだけでも，自動的にそうなっている。

コンパイルに成功すると，`$FOAM_USER_LIBBIN`に `libnewCrossPowerLaw.so` というファイルが生成される。

[［手順一覧に戻る］](#tableOfContents)


<a name="createTestCase02"></a>
## 例題の作成 create a tutarial case

先ほど作成した例題 `$FOAM_RUN/simpleCar` を使って，作成したライブラリとモデルの実行を確認する。

simpleCar 例題ディレクトリに移動する。不要な情報を削除して，まっさらな状態に戻すため，foamCleanTutorials を実行する。

> cd $FOAM_RUN/simpleCar/

> foamCleanTutorials

この状態で，先に作成した例題のtransportModelをnewCrossPowerLawに変更する。newCrossPowerLawモデルを使うように，constant/transportPropertiesを変更する。myCrossPowerLawをnewCrossPowerLawに書き換える。

> sed -i s/myCrossPowerLaw/newCrossPowerLaw/g ./constant/transportProperties

Allrunスクリプトを実行すると，次のエラーが発生する。

> ./Allrun

```cpp
--> FOAM FATAL IO ERROR: 
Unknown viscosityModel type newCrossPowerLaw

Valid viscosityModel types :

13
(
ArrheniusBirdCarreau
ArrheniusCasson
ArrheniusCrossPowerLaw
ArrheniusHerschelBulkley
ArrheniusNewtonian
BirdCarreau
Casson
CrossPowerLaw
HerschelBulkley
Newtonian
myCrossPowerLaw
powerLaw
strainRateFunction
)
```

これは，使用しているソルバをコンパイルしたとき，リンクするライブラリとして，今回作成したライブラリを設定していないためである。

再度ソルバをコンパイルするときに，リンクを修正する方法で回避できる。しかし，それでは，新たなライブラリを作るたびに，ソルバまでコンパイスし直す必要が生じる。

OpenFOAMでは，これを回避する方法が用意されている。作成したライブラリを使用するために，controlDictにライブラリを登録する方法である。下記の内容を，controlDictの最後に追加する。

```C++
libs
(
    "libnewCrossPowerLaw.so"
);
```

Allrunスクリプト を実行する。

> ./Allrun

ログを確認して，問題ないことを確認する。ログの冒頭付近に，下記の記述があり，作成したモデルが使用されていることを確認する。

```cpp
Selecting incompressible transport model newCrossPowerLaw
```

[［手順一覧に戻る］](#tableOfContents)


<a name="discussion"></a>
## 考察

<a name="whichWay"></a>
### ライブラリ丸ごとコピーと，クラス単位の改造，どちらが良いの？

手っ取り早いのは、丸ごとコピー。

ただし，標準ディレクトリとユーザーディレクトリに，重複した同一コードが多数存在する状態となる。これは，危険をはらむ。

クラス単位の場合，直接改造した部分以外は，標準のコードが使われる。思わぬコード違いが防げる。

OpenFOAMのバグ修正やアップデートがあるとき，本体のコードは修正されるが，コピーしたコードが修正されない可能性がある。（自動的には修正されないため，自身で修正する必要がある。）

個人の意見：クラス単位の改造を推奨する。バージョン変更への追随も容易になる。

<a name="turbAndtransport"></a>
### 乱流モデルとtransportModelとの違い

Make/files を見て確認する。

乱流モデルは，マクロを活用して，1組のコードから複数の種類(圧縮性/非圧縮性)のクラスを生成する。そのため，複雑に見える。

乱流モデルの生成に使われるマクロや，単独の乱流モデルをライブラリにする方法などは，下記に説明がある。
[参考サイト３](https://qiita.com/snaka-dev/items/43779235a4ed966788aa)

[［手順一覧に戻る］](#tableOfContents)


<a name="moreModification"></a>
## モデルのさらなる改造

非ニュートン粘性モデルの検証に使えるように，ひずみ速度を出力するための変更を加える。

<a name="newCrossPowerLawH"></a>
### newCrossPowerLaw.H の修正

`newCrossPowerLaw.H` の70行目付近に，volScalarFieldクラスの変数として，変数 `strainRate_` を宣言する。OpenFOAMの流儀に従って，クラス内の変数名最後にはアンダーバー_を付している。

なお，`strainRate_` の宣言は，変数 `nu_` の前に書くことが必要である。これは，後ほど確認する初期化方法に関連している。

さらに，同じファイル内で，関数 `calcNu()` のconst指定をコメントアウトする。これは，この関数内で歪み速度を計算して，変数 `strainRate_` の値を変更するためである。コードの該当部分を下にしめす。

```C++ newCrossPowerLaw.H
protected:

    // Protected data

        volScalarField strainRate_; // newly added
        volScalarField nu_;

    // Protected Member Functions

        //- Calculate and return the laminar viscosity
        tmp<volScalarField> calcNu(); //"const" is commented out.
```

<a name="newCrossPowerLawC"></a>
### newCrossPowerLaw.C の修正

`newCrossPowerLaw.C` の51行目付近で，`calcNu()` 関数のconst指定をコメントアウトする。

この関数の中で，変数 `strainRate_` に，関数 `strainRate()` で求めた値を代入する。returnする値の計算式ないで，関数 `strainRate()` を呼び出していた部分を，先に値を定めた変数 `strainRate_` に置き換える。改訂する部分を下記に示す。

なお，ここで使用する `strainRate()` 関数は，newCrossPowerLawクラスが継承している元のクラス viscosityModel で定義されている。

```C++ newCrossPowerLaw.C
Foam::tmp<Foam::volScalarField>
Foam::viscosityModels::newCrossPowerLaw::calcNu() //const//const is commented out.
{
    strainRate_ = strainRate(); 
    return (nu0_ - nuInf_)/(scalar(1) + pow(m_*strainRate_, n_)) + nuInf_;
    //return (nu0_ - nuInf_)/(scalar(1) + pow(m_*strainRate(), n_)) + nuInf_;
}
```

さらに，`newCrossPowerLaw.C` の78行目付近で，Constructor の初期化部分に，変数 `strainRate_` を加える。初期化リストは，変数を宣言した順に並べる必要がある。そのため，，`strainRate_` の初期化を，変数 `nu_` の前に記述する。

ここで，変数 `nu_` の初期化方法を見ると，`calcNu()` 関数が使われている。今回のプログラム改造では，この関数内で`strainRate_` 変数を扱うため，`strainRate_` を先に初期化した。

```C++
    strainRate_
    (
        IOobject
        (
            "strainRate",
            U_.time().timeName(),
            U_.db(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        U_.mesh(),
        dimensionedScalar("strainRate", dimVelocity/dimLength, Zero) //or strainRate()
    ),
    nu_
    (
        IOobject
        (
            name,
            U_.time().timeName(),
            U_.db(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        calcNu()
    )
```

参考のために，作成したコード類を共有する。

[newCrossPowerLaw](./src/newCrossPowerLaw/)

<a name="compileModifiedNewCrossPowerLaw"></a>
### コンパイル


```bash
cd $WM_PROJECT_USER_DIR/src/newCrossPowerLaw
wmake
```

<a name="testModifiedNewCrossPowerLaw"></a>
### 動作確認

例題を実行する。結果の時刻ディレクトリに，strainRateというファイルが書き出されていることを確認する。そのファイル内の情報が正しいことを確認する。

# サンプルファイル

[mySimpleFoam](./solvers/mySimpleFoam/)

[transportModels](./src/transportModels/)

[newCrossPowerLaw](./src/newCrossPowerLaw/)

[simpleCar](./tutorials/simpleCar/)



<details><summary>参考情報：クリックで折りたたみ表示の設定/解除が切り替えられます。</summary>

## 参考情報

### ソルバからの書き出しにfunctionObjectが使えるのか？

OpenFOAMのバージョン2.0.0から追加された runtimeCodeCompilation 機能によって，ソルバを改造せずに，歪み速度を書き出すことを検討する。下記サイトの紹介では，簡単な使用例が記載されている。

http://www.openfoam.org/version2.0.0/runtime-control.php#runtimeCodeCompilation

```
functions 
( 
    pAverage 
    { 
        functionObjectLibs ("libutilityFunctionObjects.so"); 
        type coded; 
        redirectType average; 
        outputControl outputTime; 
        code 
        #{ 
            const volScalarField& p = 
                mesh().lookupObject<volScalarField>("p"); 
            Info<<"p avg:" << average(p) << endl;
        #}; 
    } 
); 
```

OpenFOAMユーザーガイドにも，説明がある。

OpenFOAM User Guide: 6.2 Function Objects
http://cfd.direct/openfoam/user-guide/function-objects/

ソルバ内に作成されている変数を書き出すためには，writeRegisteredObject タイプを使用する。変数を新たに定義して書き出すためには，これは使えない。少し込み入ったコードを作成する必要がある。

CFD-Online に，関連するディスカッションが存在する。下記のポスト#9を参考にして，サンプルを作成した。
http://www.cfd-online.com/Forums/openfoam-programming-development/99207-create-registered-object-runtime-using-functionobject.html#post384295

```
functions 
( 
    str 
    { 
        functionObjectLibs ("libutilityFunctionObjects.so"); 
        type coded; 
        redirectType strRate; // arbitrary name

        //outputControl outputTime;    // for Info
    
        outputControl   timeStep;   // for Info
        outputInterval  1;          // strRate avg will be written every time step
    
        code 
        #{ 
            const volVectorField& U = mesh().lookupObject<volVectorField>("U");
            static autoPtr<volScalarField> pField;
    
            if(!pField.valid())
            {
                Info << "Creating strRatio" << nl;
                pField.set
                (
                        new volScalarField
                        (
                             IOobject
                             (
                                 "strRatio",
                                  mesh().time().timeName(),
                                  U.mesh(),
                                  IOobject::NO_READ,
                                  IOobject::AUTO_WRITE
                             ),
                             Foam::sqrt(2.0)*mag(symm(fvc::grad(U)))
                        )
                );
            }
    
            volScalarField &strRatio = pField();
    
            strRatio.checkIn();
    
            strRatio = Foam::sqrt(2.0)*mag(symm(fvc::grad(U)));
    
            Info<<"strRate avg:" << average(strRatio) << endl; 
    
        #}; 
    } 
); 
```

function objects に関する参考サイト

function objects
http://www.geocities.co.jp/penguinitis2002/study/OpenFOAM/function_objects.html

 Tip Function Object writeRegisteredObject
 https://openfoamwiki.net/index.php/Tip_Function_Object_writeRegisteredObject

cfd-online
 http://www.cfd-online.com/Forums/openfoam/75049-how-get-density-field-compressible-flow.html#post254920

</details>

----
[［手順一覧に戻る］](#tableOfContents)



# 作業メモ

## 環境変数

主な環境変数名と意味

| 環境変数名          | 意味                                                         |
| ------------------- | ------------------------------------------------------------ |
| WM_PROJECT_DIR      | インストールディレクトリ                                     |
| FOAM_SRC            | システムのOpenFOAMソースコード格納場所                       |
| FOAM_SOLVERS        | システムのソルバ（実行ファイル）の格納場所                   |
| FOAM_SRC            | システムのソルバ（実行ファイル）の格納場所                   |
| FOAM_TUTORIALS      | システムの例題格納場所                                       |
| FOAM_APPBIN    | ソルバ（実行ファイル）の格納場所   |
| FOAM_LIBBIN    | ライブラリ（実行ファイル）の格納場所 |
| WM_PROJECT_USER_DIR | ユーザーのOpenFOAM関連ファイル格納場所                       |
| FOAM_RUN            | ユーザーの作業ディレクトリ格納場所                           |
| FOAM_USER_APPBIN    | ユーザーがカスタマイズしたソルバ（実行ファイル）の格納場所   |
| FOAM_USER_LIBBIN    | ユーザーがカスタマイズしたライブラリ（実行ファイル）の格納場所 |

環境変数名と意味：システム・インストール領域など
| 環境変数名          | 意味                                   |
| ------------------- | ----------------------------------- |
| WM_PROJECT_DIR      | インストールディレクトリ               |
| FOAM_SRC            | システムのOpenFOAMソースコード格納場所  |
| FOAM_SOLVERS        | システムのソルバ（実行ファイル）の格納場所 |
| FOAM_TUTORIALS      | システムの例題格納場所                 |
| FOAM_APPBIN    | ソルバ（実行ファイル）の格納場所   |
| FOAM_LIBBIN    | ライブラリ（実行ファイル）の格納場所 |


環境変数名と意味：ユーザー用領域など

| 環境変数名          | 意味                                   |
| ------------------- | ----------------------------------- |
| WM_PROJECT_USER_DIR | ユーザーのOpenFOAM関連ファイル格納場所 |
| FOAM_RUN            | ユーザーの作業ディレクトリ格納場所      |
| FOAM_USER_APPBIN    | ユーザーがカスタマイズしたソルバ（実行ファイル）の格納場所 |
| FOAM_USER_LIBBIN    | ユーザーがカスタマイズしたライブラリ（実行ファイル）の格納場所 |
