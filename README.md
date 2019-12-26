# test

作者、ライセンス、バージョンなどの情報を記載
    MODULE_AUTHOR("Hayao Tamura");
    MODULE_DESCRIPTION("driver for LED control");
    MODULE_LICENSE("GPL");
    MODULE_VERSION("0.1");

static dev_t dev;
static struct cdev cdv;   ...キャラクタデバイスの情報を格納する構造体







static int __init init_mod(void)    ...カーネルモジュールの初期化
　カーネルモジュール：カーネルの機能を拡張するモジュール
printk( )  KERN_INFO  ...ログのレベルを示すマクロ
　　　　　　__FILE__   ...ソースコードのファイル名
      　　 MAJOR      ...devからメジャー番号を取り出すマクロ  
      ここで、ログを吐くように設定される
alloc_chrdev_region(&dev,0,1,"myled");  ...デバイス番号の取得
　dev：引数
  マイナー番号：0番から1番のマイナー番号
  デバイスの名前：myled

static void __exit cleanup_mod(void)
{
  unregister_chrdev_region   ...デバイス番号の解放
　unregister_chrdev_region(dev,1) ...引数dev,マイナー番号を1個返す
  これをすることによって、番号がinsmodのたびに増えてしまうことを防ぐこととなる

make
sudo insmod myled.ko
tail /var/log/messages
lsmod
sudo rmmod myled

