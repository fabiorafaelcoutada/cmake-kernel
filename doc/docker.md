
# 在 Docker 中编译并运行

## 构建 Docker image

构建 image

```
cd cmake-kernel
docker build -t cmake-kernel-docker .
```shell

## 启动容器，并配置 ssh

运行以下命令启动容器:

```
docker run --name cmake-kernel-container -itd -p 10122:22 cmake-kernel-docker
```

进入 docker container ubuntu 命令行环境

```
docker exec -it cmake-kernel-container /bin/bash
```

## 配置 ssh

在容器内增加ssh用户和密码。将下面的`yourusername`替换为你想要的用户名

```
service ssh start
useradd -m -d /home/yourusername -s /bin/bash -G sudo yourusername
passwd yourusername
```

记住 yourusername 的密码

## 配置 mac 本地使用 SSH Key 登录 ubuntu
在Mac端创建 RSA Key:

```
# 检查Mac是否已经存在RSA key:
ls ~/.ssh

# 如果已经存在id_rsa.pub则忽略后续步骤，否则继续

ssh-keygen -t rsa -b 2048 -C "<comment>"
```

在Mac运行`cat ~/.ssh/id_rsa.pub`, 然后复制内容。

以twer用户ssh进入ubuntu容器内：

```
ssh -p 10122 yourusername@localhost
# 输入twer的密码
```

创建`/home/yourusername/.ssh/authorized_keys`, 并粘贴。

从mac直接ssh 登录ubuntu:

```
ssh -p 10122 yourusername@localhost
```
此时应该能够直接登录，不再需要输入密码

# 下载代码，配置vscode

## 在ubuntu创建ssh key，用于gitlab

在mac端ssh进入ubuntu，在ubuntu端创建ssh key:

```
ssh-keygen -t ed25519 -C "for gitlab"
# 然后一直回车即可
```

然后copykey内容：
```
cat ~/.ssh/id_ed25519.pub
```

粘贴到gitlab: "User Settings/SSH Keys", add key

## clone代码：

ssh进入ubuntu容器，clone代码

```
mkdir -p /home/yourusername/projects
cd /home/yourusername/projects
git clone git@10.108.6.233:tw-taskmanager/taskmanager.git
```

## 检查在容器内编译和运行测试可以正常工作

```
cd taskmanager
cd scripts
./build.sh
./unit_test.sh
./system_test.sh
```

## 配置VSCode

* 安装`Remote - SSH` 插件
* 打开Command Palette: CMD+Shift+P
* 输入`Remote-SSH: Add New SSH Host...`
* 输入SSH命令： `ssh -p 10122 yourusername@localhost`
* 登录成功后，在打开的窗口中打开文件夹。
