#!/usr/bin/python3
import os
import subprocess
import time
import sys
import math

class Timeout(Exception):
	pass

def run(command,timeout = 10):
	if isinstance(command,str):
		command = command.split()
	proc = subprocess.Popen(command,bufsize=0,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	poll_seconds = .1
	deadline = time.time()+timeout
	while time.time() < deadline and proc.poll()==None:
		time.sleep(poll_seconds)

	if proc.poll()==None:
		proc.terminate
		raise Timeout()

	stdout,stderr = proc.communicate()
	return stdout,stderr,proc.returncode

stdout,stderr,status = run("VBoxManage list runningvms")
if "zos" in stdout.decode('u8'):
	run("VBoxManage controlvm zos poweroff")

fin = open("/home/zihao/osdev/boot.img",'rb')
data = fin.read()
fin.close()
fout = open("/home/zihao/VirtualBox VMs/zos/zos-flat.vmdk",'wb')
length = fout.write(data)

if length%512 !=0:
	fout.write(b'\x00'* (512-length%512))
fout.write(b'\x00' * (1024*512))
fout.close()

run("VBoxManage startvm zos")