# Filter

## はじめに

このフォルダには [CLIPSTUDIO PAINT フィルタープラグイン SDK (ソフトウェア開発キット)][cspsdk] を C++ で使いやすいようにラップしたユーティリティーの使い方を説明するためのサンプルが入っています。

## ビルド方法

1. cspsdkxx と同じ階層に [CLIPSTUDIO PAINT フィルタープラグイン SDK (ソフトウェア開発キット)][cspsdk] のフォルダを配置します。

1. macOS の場合、[Xcode][xcode] を起動して xcodeproj/Filter.xcodeproj を開いてビルドします。
  Xcode 9 でビルドできることを確認しています。

1. Windows の場合、[Visual Studio][vstudio] を起動して vcxproj/Filter.sln を開いてビルドします。
  Visual Studio Professional 2017 でビルドできることを確認しています。


[cspsdk]:https://www.clip-studio.com/clip_site/download/clipstudiopaint/cspsdk
[xcode]:https://itunes.apple.com/jp/app/xcode/id497799835
[vstudio]:https://www.microsoft.com/ja-jp/dev/products/community.aspx
