황영덕 강사


/media/sf_BSP-mds2450 << mount disk 폴더


arch_number = 0x00000DA7 << 보드 번호
env_t       = 0x00000000
boot_params = 0x30000100 << 인자 저장 위치
DRAM bank   = 0x00000000
-> start    = 0x30000000 << RAM의 시작 ADDRESS
-> size     = 0x04000000 
ethaddr     = 00:40:5C:26:0A:5B
ip_addr     = 192.168.20.111
baudrate    = 115200 bps

보드 인포메이션

# 보드 기본 설정
setenv 인자 값 <<  설정
printenv  << 설정 보기
saveenv << flash에 기록

#printenv 초기 설정
baudrate=115200
ethaddr=00:40:5c:26:0a:5b
ipaddr=192.168.20.111
serverip=192.168.20.90
gatewayip=192.168.20.1
netmask=255.255.255.0
boot=test
bootargs=root=/dev/nfs rw nfsroot=192.168.20.90:/nfs/rootfs ip=192.168.20.246:192.168.20.90:192.168.20.1:255.255.255.0::eth0:off console=ttySAC1,115200n81
bootcmd=dnw 30000000; go 30000000
bootdelay=1
stdin=serial
stdout=serial
stderr=serial

bootcmd는 자동 호출되는 명령임 (bootdelay=1이 종료되면 실행)

# LCD가 포함된 보드 보드인포
MDS2450#
arch_number = 0x00000DA7
env_t       = 0x00000000
boot_params = 0x30000100
DRAM bank   = 0x00000000
-> start    = 0x30000000
-> size     = 0x04000000
ethaddr     = 00:40:5C:26:0A:5B
ip_addr     = 192.168.20.246
baudrate    = 115200 bps
MDS2450#


# LCD 포함 보드 초기 환경
MDS2450# printenv
baudrate=115200
ethaddr=00:40:5c:26:0a:5b
boot=test
bootdelay=5
bootargs_backup=root=/dev/nfs rw nfsroot=192.168.20.90:/nfsroot ip=192.168.20.246:192.168.20.90:192.168.20.1:255.255.255.0::eth0:off console=ttySAC1,115200n81
filesize=BF61D7
fileaddr=30800000
gatewayip=192.168.20.1
netmask=255.255.255.0
ipaddr=192.168.20.246
serverip=192.168.20.90
bootcmd=tftp c0008000 zImage; bootm c0008000 << tftp명령어로 zImage 가져옴(from PC), bootm으로 해당 번지로 점프, c는 메모리 주소가 MMU에 의해 3->c로 변경됨(보드인포 참조)
bootargs=root=/dev/nfs rw nfsroot=192.168.20.90:/work/rootfs ip=192.168.20.246:192.168.20.90:192.168.20.1:255.255.255.0::eth0:off console=ttySAC1,115200n81
stdin=serial
stdout=serial
stderr=serial

# 강사랑 align 맞추기
set bootargs 'root=/dev/nfs rw nfsroot=192.168.20.90:/work/rootfs ip=192.168.20.246:192.168.20.90:192.168.20.1:255.255.255.0::eth0:off console=ttySAC1,115200n81'
set serverip 192.168.20.90
set ipaddr 192.168.20.111
set gatewayip 192.168.20.1
set netmask 255.255.255.0
set bootcmd 'tftp c0008000 zImage; bootm c0008000'


## 부트로더 빌디 방법
ARCH, CROSS_COMPILE 설정
make distclean << 설정 및 바이너리 삭제
make <target>_config << 설정 로드(Makefile)
make mds2450_config 
make << u-boot.bin 생성


#gcc 옵션
-g 디버깅심볼 추가 << gdb같은 디버거로 심벌 파악 가능
-O 최적화 레벨
-o 컴파일 및 링킹 후 이름
-I<헤더> 헤더의 위치
-D<선언> 선언
-F<옵션> 컴파일러 옵션
-l<라이브러리> 링킹할 라이브러리
-L<라이브러리> 라이브러리 위치

# u-boot 포팅관련 파일(s3c2450)
include/configs/mds2450.h << 보드 헤더(각종 설정)
include/regs.h << SFR정의(심볼릭 링크)
cpu/arm926ejf << 사용안함
cpu/s3c24xx << start.S 스타트업
		serial.c, usb... 기본 장치등
board/mds/mds2450/
	config.mk << TEXT_BASE(코드 영역 위치)
	u-boot.lds << 섹션 배치
	mds.2450.c << 보드 파일

#머신 번호(보드 번호)
/work/kernel-mds2450-3.0.22/arch/arm/tools/mach-types
mds2450     MACH_MDS2450        MDS2450     3495

#NAND에u-boot 퓨징
nand erase 0 40000 << erase [시작주소] [사이즈]
tftp c0008000 u-boot.bin 
nand write c30080000 0 40000 << write [읽어올 주소] [기록주소] [사이즈]

#mds보드 커널 기본 설정
빌드 전 arch, cross_compile 설정이 되어 있어야 함(.bashrc)
arch/*/configs/mds2450_defconfig

make distclean << 설정, 객체 삭제
make mds2450_config << 기본설정 로드
make menuconfig << 세부설정(TUI) >> Kconfig (메뉴파일)
		   xxx [ * ] << zImage에 포함
                   xxx [ M ] << 커널 모듈
make zImage << 커널만 빌드
make modules  << *.ko만 빌드
make -j[빌드 스레드 수] 

# Kernel 핵심파일
Board 파일 /커널/arch/arm/mach-s3c2416/mach-mds2450.c
- zImage 배치위치 /work/kernel-mds2450-3.0.22/arch/arm/mach-s3c2410/Makefile.boot
- ARM관련 head.S 파일 위치 /work/kernel-mds2450-3.0.22/arch/arm/kernel/head.S
- arch/arm/kernel/vmlinux.lds << 커널의 ENTRY(stext)

# ctags 사용법
- 외부에서 탐색
vim -t [원하는 심볼]
- 내부에서 탐색
:ts [심볼명] << 태그목록 탐색(중복시)
:ta [심볼명] << 단일 탐색
ctrl + ] << 커서 아래에 있는 심볼 탐색
ctrl + t << 되돌아 오기
:Tlist << 창에 태그목록 보여주기

# 프로세스 정보 보기
ps -o pid,ppid, stat, vsz, rss, comm
      [식별][부모][상태][가상][실제][이름]
       프로세스 tgid
       스레드 tid
vsz (vm의 사이즈 -> 공유된 영역 + 로드된 크기)
rss 공유 제외한 실제 로드 사이즈 << resident set size

# ps -o pid,ppid,stat,vsz,rss,comm << 입력한 순서대로 보기


# 실행
cp sk.ko sk_app /work/rootfs
테라텀
insmod sk.ko
car /proc/devices
mknod /dev/sk c 251 0
ls -l /dev/sk
./sk_app
Makefile에 추가arm-unknown-linix-gnueabi-gcc sk_app.c -o sk_app

# 디바이스 포팅
1. 디바이스 동작 파악
 - 디바이스 데이터 시트, 회로도
   GPG 4,5,6,7 << LED 4,5,6,7
   동작 :1 -> OFF, 0 -> ON
2. SFR의 주소 파악
 - GPIO의 G레지스터 찾기 -> SoC 메뉴얼
   GPGCON 0x56000060 << 제어(in, out, 인터럽트 모드 설정)
   [15:0] 까지 제어 함 (4[9:8],5[11:10]...,6,7)
   -> OUT으로 설정 따라서, 01입력
   GPGDAT 0x56000064 << data 입력(1,0)
   [7:4]에 data 입력
   GPGUDP 0x56000068 << 
3. 커널 API혹은 함수 설계
 - HW접근용 : readl(), writel(), ioread32(), iowrite32()
          <<, >> 비트시프트, | OR, & AND, ~ NOT 등의 비트 연산이 자유로워야 함
 - GPIO용 함수
   gpio_request() << 사용
   gpio_set_value() << 값 쓰기
   gpio_get_value() << 값 읽기
 -s3c_XXX() << s3c용 API : get, set... 의존성 있음

4.드라이버 설계
 - 헤더, (접근, SFR헤더)
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>


KDIR    := /work/kernel-mds2450-3.0.22

all:
    make -C $(KDIR) SUBDIRS=$(PWD) modules
    arm-unknown-linux-gnueabi-gcc sk_app.c -o sk_app    


