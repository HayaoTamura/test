#再提出　デバイスドライバ　課題
LEDをチカチカさせることができるようにmyled.cを変更して、
myled2.cで表示しました。




# デバイスドライバ　解説　課題

ロボットシステム学課題1において作成したデバイスドライバの内容の解説を行う。
    
    
　　　MODULE_AUTHOR("Hayao Tamura");
    
　　　MODULE_DESCRIPTION("driver for LED control");
    
　　　MODULE_LICENSE("GPL");
    
　　　MODULE_VERSION("0.1");　　...作者、ライセンス、バージョンなどの情報を記載

GPL　: コピーレフトの考え方をライセンス化したもの。基本的なルールとして、GPLで配布されたプログラムについて、ユーザーは実行、再配布、改変、調査できる権利を保持する。ただし、再配布の際には、再度GPLで配布する義務がある。

GPLの効果として、1人が作れば他の人が苦労して同じものを作る必要がなくなるという大きな利点がある。また、人のコードを持ってきた企業や団体などが改変部分を隠してコピーを売るというようなことは抑制される。

static dev_t dev;

static struct cdev cdv;   ...キャラクタデバイスの情報を格納する構造体

static ssize_t led_write(struct file* flip,const char* buf,size_t count,loff_t*pos)

led_write   ...デバイスファイルに書き込みがあった時の挙動

static struct file_operations led_fops   ...挙動を書いた関数のポインタを格納する構造体

char c;   ...読み込んだ字を入れる変数

if(copy_from_user(&c,buf,sizeof(char)))　　...copy_from_userという関数

GPFSET0のGPIO25に対応するところに1を書き込みONの設定

GPCLR0のGPIO25に対応すると黒に1を書き込みOFFの設定

static ssize_t sushi_read(struct file* filp,char* buf,size_t count,loff_t*pos)

...catするとsushiを表示する機能の実装

if(copy_to_user(buf+size,(const char *)sushi,sizeof(sushi))){

printk(KERN_INFO "sushi : copy_to_user failed\n");

copy_to_user   ...カーネルからユーザランドへ文字を送る

ユーザランド : OSが動作するのに必要なカーネル以外の部分を指す

ファイルシステムやファイル操作コマンド、シェルなどの基本的なソフトウェア群を指す



static int __ init init_mod(void)    ...カーネルモジュールの初期化

カーネルモジュール：カーネルの機能を拡張するモジュール

gpio_base = ioremap_nocache(0x3f200000,0xA0);　　

...0x3f200000 : GPIOのレジスタの最初のアドレス

0xA0 : 必要なアドレスの範囲

ピンを出力するレジスタに1を書き込む

printk( )  

KERN_INFO  ...ログのレベルを示すマクロ

__FILE__   ...ソースコードのファイル名

MAJOR      ...devからメジャー番号を取り出すマクロ  

ここで、ログを吐くように設定される

alloc_chrdev_region(&dev,0,1,"myled");  ...デバイス番号の取得

dev：引数

マイナー番号：0番から1番のマイナー番号

デバイスの名前：myled

cdev_init(&cdv, &led_fops);　　　...キャラクタデバイスの初期化

retval = cdev_add(&cdv, dev, 1);  ...cdev_add : キャラクタデバイスをカーネルに登録

cls = class_create(THIS_MODULE,"myled");　　...class_createでの作成

THIS_MODULE   ...このモジュールを管理するこう構造体のポインタ

device_create(cls,NULL,dev,NULL,"myled%d",MINOR(dev));

device_create  ...デバイス情報の作成





static void __exit cleanup_mod(void)
{

cdev_del(&cdv);　...キャラクタデバイスの破棄

device_destroy(cls,dev);   ...デバイス情報の削除

class_destroy　　...クラスの削除

unregister_chrdev_region   ...デバイス番号の解放

unregister_chrdev_region(dev,1) ...引数dev,マイナー番号を1個返す
  これをすることによって、番号がinsmodのたびに増えてしまうことを防ぐこととなる



