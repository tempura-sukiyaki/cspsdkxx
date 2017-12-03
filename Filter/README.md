# Filter

## はじめに

このフォルダには [CLIPSTUDIO PAINT フィルタープラグイン SDK (ソフトウェア開発キット)][cspsdk] を C++ で使いやすいようにラップしたユーティリティーの使い方を説明するためのサンプルが入っています。

## ビルド方法

1. cspsdkxx と同じ階層に [CLIPSTUDIO PAINT フィルタープラグイン SDK (ソフトウェア開発キット)][cspsdk] のフォルダを配置します。

1. macOS の場合、[Xcode][xcode] を起動して xcodeproj/Filter.xcworkspace または xcodeproj/All.xcodeproj を開いてビルドします。
  Xcode 9.1 でビルドできることを確認しています。

1. Windows の場合、[Visual Studio][vstudio] を起動して vcxproj/Filter.sln を開いてビルドします。
  Visual Studio Professional 2017 でビルドできることを確認しています。

## Xcode でのデバッグ方法

1. メインメニューから [Product] - [Scheme] - [All] を選択します (All が選択されていない場合)。

1. メインメニューから　[Product] - [Scheme] - [Edit Scheme] を選択します。

1. 左の項目から [Run] を選択して、[Info] タブを選択します。

1. [Executable] から "Other..." を選択して、CLIP STUDIO PAINT EX の実行ファイルを指定して、ダイアログを閉じます。

1. メインメニューから [Product] - [Run] を選択します。

## Visual Studio でのデバッグ方法

1. ソリューション エクスプローラーの "All" のコンテキストメニューから [スタートアップ プロジェクトに設定] を選択します (All がスタートアップ プロジェクトでない場合)。

1. ソリューション エクスプローラーの "All" のコンテキストメニューから [プロパティ] を選択します。

1. 左の項目から [構成プロパティ] - [デバッグ] を選択します。

1. [コマンド] に CLIP STUDIO PAINT EX のパスを指定して、ダイアログを閉じます (大体の場合 "$(ProgramW6432)\CELSYS\CLIP STUDIO 1.5\CLIP STUDIO PAINT\CLIPStudioPaint.exe" です)。

1. メインメニューから [デバッグ] - [デバッグの開始] を選択します。


[cspsdk]:https://www.clip-studio.com/clip_site/download/clipstudiopaint/cspsdk
[xcode]:https://itunes.apple.com/jp/app/xcode/id497799835
[vstudio]:https://www.microsoft.com/ja-jp/dev/products/community.aspx
