#!/system/bin/sh
export PATH=/system/xbin:$PATH

if [ ! -f /cache/pds-CM.img ]
then
    #make a copy of pds in /cache
    /system/bin/dd if=/dev/block/platform/msm_sdcc.1/by-name/pds of=/cache/pds-CM.img
    echo "Backed up PDS"
fi

#mount the fake pds
/system/bin/losetup /dev/block/loop0 /cache/pds-CM.img
/system/bin/mount -o rw -t ext3 /dev/block/loop0 /pds
echo "Mounted PDS"
