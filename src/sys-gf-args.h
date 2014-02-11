/*
  Copyright (c) 2012-2013 DataLab, S.L. <http://www.datalab.es>

  This file is part of the gfsys library for GlusterFS.

  The gfsys library for GlusterFS is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The gfsys library for GlusterFS is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the gfsys library for GlusterFS. If not, see
  <http://www.gnu.org/licenses/>.
*/

#ifndef __SYS_GF_ARGS_H__
#define __SYS_GF_ARGS_H__

#define SYS_GF_ID_access       GF_FOP_ACCESS
#define SYS_GF_ID_create       GF_FOP_CREATE
#define SYS_GF_ID_discard      GF_FOP_DISCARD
#define SYS_GF_ID_entrylk      GF_FOP_ENTRYLK
#define SYS_GF_ID_fentrylk     GF_FOP_FENTRYLK
#define SYS_GF_ID_fallocate    GF_FOP_FALLOCATE
#define SYS_GF_ID_flush        GF_FOP_FLUSH
#define SYS_GF_ID_fsync        GF_FOP_FSYNC
#define SYS_GF_ID_fsyncdir     GF_FOP_FSYNCDIR
#define SYS_GF_ID_getspec      GF_FOP_GETSPEC
#define SYS_GF_ID_getxattr     GF_FOP_GETXATTR
#define SYS_GF_ID_fgetxattr    GF_FOP_FGETXATTR
#define SYS_GF_ID_inodelk      GF_FOP_INODELK
#define SYS_GF_ID_finodelk     GF_FOP_FINODELK
#define SYS_GF_ID_link         GF_FOP_LINK
#define SYS_GF_ID_lk           GF_FOP_LK
#define SYS_GF_ID_lookup       GF_FOP_LOOKUP
#define SYS_GF_ID_mkdir        GF_FOP_MKDIR
#define SYS_GF_ID_mknod        GF_FOP_MKNOD
#define SYS_GF_ID_open         GF_FOP_OPEN
#define SYS_GF_ID_opendir      GF_FOP_OPENDIR
#define SYS_GF_ID_rchecksum    GF_FOP_RCHECKSUM
#define SYS_GF_ID_readdir      GF_FOP_READDIR
#define SYS_GF_ID_readdirp     GF_FOP_READDIRP
#define SYS_GF_ID_readlink     GF_FOP_READLINK
#define SYS_GF_ID_readv        GF_FOP_READV
#define SYS_GF_ID_removexattr  GF_FOP_REMOVEXATTR
#define SYS_GF_ID_fremovexattr GF_FOP_FREMOVEXATTR
#define SYS_GF_ID_rename       GF_FOP_RENAME
#define SYS_GF_ID_rmdir        GF_FOP_RMDIR
#define SYS_GF_ID_setattr      GF_FOP_SETATTR
#define SYS_GF_ID_fsetattr     GF_FOP_FSETATTR
#define SYS_GF_ID_setxattr     GF_FOP_SETXATTR
#define SYS_GF_ID_fsetxattr    GF_FOP_FSETXATTR
#define SYS_GF_ID_stat         GF_FOP_STAT
#define SYS_GF_ID_fstat        GF_FOP_FSTAT
#define SYS_GF_ID_statfs       GF_FOP_STATFS
#define SYS_GF_ID_symlink      GF_FOP_SYMLINK
#define SYS_GF_ID_truncate     GF_FOP_TRUNCATE
#define SYS_GF_ID_ftruncate    GF_FOP_FTRUNCATE
#define SYS_GF_ID_unlink       GF_FOP_UNLINK
#define SYS_GF_ID_writev       GF_FOP_WRITEV
#define SYS_GF_ID_xattrop      GF_FOP_XATTROP
#define SYS_GF_ID_fxattrop     GF_FOP_FXATTROP

#define SYS_GF_ARGS_FOP                                                    \
        (call_frame_t *,     frame),                                       \
        (xlator_t *,         this)

#define SYS_GF_ARGS_CBK                                                    \
        (call_frame_t *,     frame),                                       \
        (void *,             cookie),                                      \
        (xlator_t *,         this),                                        \
        (int32_t,            op_ret),                                      \
        (int32_t,            op_errno)

#define SYS_GF_ARGS_access                                                 \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (int32_t,            mask),                                        \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_create                                                 \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (int32_t,            flags),                                       \
        (mode_t,             mode),                                        \
        (mode_t,             umask),                                       \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_discard                                                \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (off_t,              offset),                                      \
        (size_t,             len),                                         \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_entrylk                                                \
        (const char *,       volume,           COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (const char *,       basename,         COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (entrylk_cmd,        cmd),                                         \
        (entrylk_type,       type),                                        \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fentrylk                                               \
        (const char *,       volume,           COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (const char *,       basename,         COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (entrylk_cmd,        cmd),                                         \
        (entrylk_type,       type),                                        \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fallocate                                              \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (int32_t,            keep_size),                                   \
        (off_t,              offset),                                      \
        (size_t,             len),                                         \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_flush                                                  \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsync                                                  \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (int32_t,            datasync),                                    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsyncdir                                               \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (int32_t,            datasync),                                    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_getspec                                                \
        (char *,             key,              COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (int32_t,            flag)

#define SYS_GF_ARGS_getxattr                                               \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (const char *,       name,             COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fgetxattr                                              \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (const char *,       name,             COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_inodelk                                                \
        (const char *,       volume,           COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (int32_t,            cmd),                                         \
        (struct gf_flock,    flock,            PTR,  sys_flock_acquire,    \
                                                     sys_flock_release),   \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_finodelk                                               \
        (const char *,       volume,           COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (int32_t,            cmd),                                         \
        (struct gf_flock,    flock,            PTR,  sys_flock_acquire,    \
                                                     sys_flock_release),   \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_link                                                   \
        (loc_t,              oldloc,           PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (loc_t,              newloc,           PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_lk                                                     \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (int32_t,            cmd),                                         \
        (struct gf_flock,    flock,            PTR,  sys_flock_acquire,    \
                                                     sys_flock_release),   \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_lookup                                                 \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_mkdir                                                  \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (mode_t,             mode),                                        \
        (mode_t,             umask),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_mknod                                                  \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (mode_t,             mode),                                        \
        (dev_t,              rdev),                                        \
        (mode_t,             umask),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_open                                                   \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (int32_t,            flags),                                       \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_opendir                                                \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_rchecksum                                              \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (off_t,              offset),                                      \
        (int32_t,            len),                                         \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readdir                                                \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (size_t,             size),                                        \
        (off_t,              offset),                                      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readdirp                                               \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (size_t,             size),                                        \
        (off_t,              offset),                                      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readlink                                               \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (size_t,             size),                                        \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readv                                                  \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (size_t,             size),                                        \
        (off_t,              offset),                                      \
        (uint32_t,           flags),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_removexattr                                            \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (const char *,       name,             COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fremovexattr                                           \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (const char *,       name,             COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_rename                                                 \
        (loc_t,              oldloc,           PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (loc_t,              newloc,           PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_rmdir                                                  \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (int,                xflags),                                      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_setattr                                                \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (struct iatt,        stbuf,            PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (int32_t,            valid),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsetattr                                               \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (struct iatt,        stbuf,            PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (int32_t,            valid),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_setxattr                                               \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (dict_t *,           dict,             COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (int32_t,            flags),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsetxattr                                              \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           dict,             COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (int32_t,            flags),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_stat                                                   \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fstat                                                  \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_statfs                                                 \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_symlink                                                \
        (const char *,       linkname,         COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (mode_t,             umask),                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_truncate                                               \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (off_t,              offset),                                      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_ftruncate                                              \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (off_t,              offset),                                      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_unlink                                                 \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (int,                xflags),                                      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_writev                                                 \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (sys_iovec_t,        vector,           IOV,  sys_iovec_acquire,    \
                                                     sys_iovec_release),   \
        (off_t,              offset),                                      \
        (uint32_t,           flags),                                       \
        (struct iobref *,    iobref,           COPY, sys_iobref_acquire,   \
                                                     sys_iobref_release),  \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_xattrop                                                \
        (loc_t,              loc,              PTR,  sys_loc_acquire,      \
                                                     sys_loc_release),     \
        (gf_xattrop_flags_t, optype),                                      \
        (dict_t *,           xattr,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fxattrop                                               \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (gf_xattrop_flags_t, optype),                                      \
        (dict_t *,           xattr,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_access_cbk                                             \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_create_cbk                                             \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (inode_t *,          inode,            COPY, sys_inode_acquire,    \
                                                     sys_inode_release),   \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_discard_cbk                                            \
        (struct iatt,        preop_stbuf,      PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postop_stbuf,     PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_entrylk_cbk                                            \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fentrylk_cbk                                           \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fallocate_cbk                                          \
        (struct iatt,        preop_stbuf,      PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postop_stbuf,     PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_flush_cbk                                              \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsync_cbk                                              \
        (struct iatt,        prebuf,           PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postbuf,          PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsyncdir_cbk                                           \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_getspec_cbk                                            \
        (char *,             spec_data,        COPY, sys_str_acquire,      \
                                                     sys_str_release)

#define SYS_GF_ARGS_getxattr_cbk                                           \
        (dict_t *,           dict,             COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fgetxattr_cbk                                          \
        (dict_t *,           dict,             COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_inodelk_cbk                                            \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_finodelk_cbk                                           \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_link_cbk                                               \
        (inode_t *,          inode,            COPY, sys_inode_acquire,    \
                                                     sys_inode_release),   \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_lk_cbk                                                 \
        (struct gf_flock,    flock,            PTR,  sys_flock_acquire,    \
                                                     sys_flock_release),   \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_lookup_cbk                                             \
        (inode_t *,          inode,            COPY, sys_inode_acquire,    \
                                                     sys_inode_release),   \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release)

#define SYS_GF_ARGS_mkdir_cbk                                              \
        (inode_t *,          inode,            COPY, sys_inode_acquire,    \
                                                     sys_inode_release),   \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_mknod_cbk                                              \
        (inode_t *,          inode,            COPY, sys_inode_acquire,    \
                                                     sys_inode_release),   \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_open_cbk                                               \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_opendir_cbk                                            \
        (fd_t *,             fd,               COPY, sys_fd_acquire,       \
                                                     sys_fd_release),      \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_rchecksum_cbk                                          \
        (uint32_t,           weak_checksum),                               \
        (uint8_t *,          strong_checksum,  COPY, sys_chksum_acquire,   \
                                                     sys_chksum_release),  \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readdir_cbk                                            \
        (gf_dirent_t,        entries,          PTR,  sys_dirent_acquire,   \
                                                     sys_dirent_release),  \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readdirp_cbk                                           \
        (gf_dirent_t,        entries,          PTR,  sys_dirent_acquire,   \
                                                     sys_dirent_release),  \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readlink_cbk                                           \
        (const char *,       path,             COPY, sys_str_acquire,      \
                                                     sys_str_release),     \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_readv_cbk                                              \
        (sys_iovec_t,        vector,           IOV,  sys_iovec_acquire,    \
                                                     sys_iovec_release),   \
        (struct iatt,        stbuf,            PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iobref *,    iobref,           COPY, sys_iobref_acquire,   \
                                                     sys_iobref_release),  \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_removexattr_cbk                                        \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fremovexattr_cbk                                       \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_rename_cbk                                             \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        preoldparent,     PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postoldparent,    PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        prenewparent,     PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postnewparent,    PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_rmdir_cbk                                              \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_setattr_cbk                                            \
        (struct iatt,        preop_stbuf,      PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postop_stbuf,     PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsetattr_cbk                                           \
        (struct iatt,        preop_stbuf,      PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postop_stbuf,     PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_setxattr_cbk                                           \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fsetxattr_cbk                                          \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_stat_cbk                                               \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fstat_cbk                                              \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_statfs_cbk                                             \
        (struct statvfs,     buf,              PTR,  sys_statvfs_acquire,  \
                                                     sys_statvfs_release), \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_symlink_cbk                                            \
        (inode_t *,          inode,            COPY, sys_inode_acquire,    \
                                                     sys_inode_release),   \
        (struct iatt,        buf,              PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_truncate_cbk                                           \
        (struct iatt,        prebuf,           PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postbuf,          PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_ftruncate_cbk                                          \
        (struct iatt,        prebuf,           PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postbuf,          PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_unlink_cbk                                             \
        (struct iatt,        preparent,        PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postparent,       PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_writev_cbk                                             \
        (struct iatt,        prebuf,           PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (struct iatt,        postbuf,          PTR,  sys_iatt_acquire,     \
                                                     sys_iatt_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_xattrop_cbk                                            \
        (dict_t *,           xattr,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

#define SYS_GF_ARGS_fxattrop_cbk                                           \
        (dict_t *,           xattr,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release),    \
        (dict_t *,           xdata,            COPY, sys_dict_acquire,     \
                                                     sys_dict_release)

/*
#define SYS_GF_ARGS_forget                                                 \
        (inode_t *,          inode)

#define SYS_GF_ARGS_invalidate                                             \
        (inode_t *,          inode)

#define SYS_GF_ARGS_release                                                \
        (fd_t *,             fd)

#define SYS_GF_ARGS_releasedir                                             \
        (fd_t *,             fd)
*/

#endif /* __SYS_GF_ARGS_H__ */

