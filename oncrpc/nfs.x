   /*
    * The maximum number of bytes of data in a READ or WRITE
    * request.
    */
   const MAXDATA = 8192;

   /* The maximum number of bytes in a pathname argument. */
   const MAXPATHLEN = 1024;

   /* The maximum number of bytes in a file name argument. */
   const MAXNAMLEN = 255;

   /* The size in bytes of the opaque "cookie" passed by READDIR. */
   const COOKIESIZE  = 4;

   /* The size in bytes of the opaque file handle. */
   const FHSIZE = 32;




/*typedef opaque fhandle[FHSIZE];*/

struct nfscookie {
   int a;
};

struct nfsdata {
   int a;
};

struct fhandle {
   int a;
};






typedef string path<MAXPATHLEN>;

 enum ftype {
              NFNON = 0,
              NFREG = 1,
              NFDIR = 2,
              NFBLK = 3,
              NFCHR = 4,
              NFLNK = 5
          };


struct fattr {
    ftype        type;
    unsigned int mode;
    unsigned int nlink;
    unsigned int uid;
    unsigned int gid;
    unsigned int size;
    unsigned int blocksize;
    unsigned int rdev;
    unsigned int blocks;
    unsigned int fsid;
    unsigned int fileid;
    timeval      atime;
    timeval      mtime;
    timeval      ctime;
};



enum stat {
  NFS_OK=0,               /* No error */
  NFSERR_PERM=1,          /* Not owner */
  NFSERR_NOENT=2,         /* No such file or directory */
  NFSERR_IO=5,            /* I/O error */
  NFSERR_NXIO=6,          /* No such device or address */
  NFSERR_ACCES=13,        /* Permission denied */
  NFSERR_EXIST=17,        /* File exists */
  NFSERR_NODEV=19,        /* No such device */
  NFSERR_NOTDIR=20,       /* Not a directory*/
  NFSERR_ISDIR=21,        /* Is a directory */
  NFSERR_FBIG=27,         /* File too large */
  NFSERR_NOSPC=28,        /* No space left on device */
  NFSERR_ROFS=30,         /* Read-only file system */
  NFSERR_NAMETOOLONG=63,  /* File name too long */
  NFSERR_NOTEMPTY=66,     /* Directory not empty */
  NFSERR_DQUOT=69,        /* Disc quota exceeded */
  NFSERR_STALE=70,        /* Stale NFS file handle */
  NFSERR_WFLUSH=99        /* Write cache flushed */
};


union readres switch (stat status) {
    case NFS_OK:
        fattr attributes;
        /*nfsdata data;*/
    default:
        void;
};
           /* The maximum number of bytes in a pathname argument. */
           const MNTPATHLEN = 1024;

           /* The maximum number of bytes in a name argument. */
           const MNTNAMLEN = 255;

           /* The size in bytes of the opaque file handle. */
           const FHSIZE = 32;


         /*
            * Remote file service routines
            */
           program NFS_PROGRAM {
                   version NFS_VERSION {
                           void
                           NFSPROC_NULL(void)              = 0;

                           attrstat
                           NFSPROC_GETATTR(fhandle)        = 1;

                           attrstat
                           NFSPROC_SETATTR(sattrargs)      = 2;

                           void
                           NFSPROC_ROOT(void)              = 3;

                           diropres
                           NFSPROC_LOOKUP(diropargs)       = 4;

                           readlinkres
                           NFSPROC_READLINK(fhandle)       = 5;

                           readres
                           NFSPROC_READ(readargs)          = 6;

                           void
                           NFSPROC_WRITECACHE(void)        = 7;


                           attrstat
                           NFSPROC_WRITE(writeargs)        = 8;

                           diropres
                           NFSPROC_CREATE(createargs)      = 9;

                           stat
                           NFSPROC_REMOVE(diropargs)       = 10;

                           stat
                           NFSPROC_RENAME(renameargs)      = 11;

                           stat
                           NFSPROC_LINK(linkargs)          = 12;

                           stat
                           NFSPROC_SYMLINK(symlinkargs)    = 13;

                           diropres
                           NFSPROC_MKDIR(createargs)       = 14;

                           stat
                           NFSPROC_RMDIR(diropargs)        = 15;

                           readdirres
                           NFSPROC_READDIR(readdirargs)    = 16;

                           statfsres
                           NFSPROC_STATFS(fhandle)         = 17;
                   } = 2;
           } = 100003;




typedef string filename<MAXNAMLEN>;

struct diropargs {
    fhandle  dir;
    filename name;
};

struct diropokStruct{
  fhandle file;
  fattr   attributes;
};

union diropres switch (stat status) {
    case NFS_OK:
        diropokStruct diropok;
    default:
        void;
};

union attrstat switch (stat status) {
    case NFS_OK:
        fattr attributes;
    default:
        void;
};

union readlinkres switch (stat status) {
       case NFS_OK:
           path data;
       default:
           void;
};





struct readargs {
       fhandle file;
       unsigned offset;
       unsigned count;
       unsigned totalcount;
};


struct readokres {
  fattr attributes;       /* attributes */
  opaque data <NFS_MAXDATA>;
};

  struct sattr {
              unsigned int mode;
              unsigned int uid;
              unsigned int gid;
              unsigned int size;
              timeval      atime;
              timeval      mtime;
          };


struct sattrargs {
    fhandle file;
    sattr attributes;
};


struct symlinkargs {
    diropargs from;
    path to;
    sattr attributes;
};


struct createargs {
         diropargs where;
         sattr attributes;
 };



      struct writeargs {
                    fhandle file;
                    unsigned beginoffset;
                    unsigned offset;
                    unsigned totalcount;
                    nfsdata data;
            };


           struct readdirargs {
                   fhandle dir;
                   nfscookie cookie;
                   unsigned count;
           };

           struct entry {
                   unsigned fileid;
                   filename name;
                   nfscookie cookie;
                   entry *nextentry;
           };

struct readdirokStruct{
       entry *entries;
       bool eof;
} ;

union readdirres switch (stat status) {
    case NFS_OK:
           readdirokStruct readdirok;
    default:
           void;
};


struct infoStruct{
   unsigned tsize;
   unsigned bsize;
   unsigned blocks;
   unsigned bfree;
   unsigned bavail;
} ;

union statfsres switch (stat status) {
    case NFS_OK:
       infoStruct info;
    default:
           void;
};


 struct linkargs {
                   fhandle from;
                   diropargs to;
           };


   struct renameargs {
           diropargs from;
           diropargs to;
   };


