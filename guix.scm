;;; Copyright © 2020 Denis Carikli <GNUtoo@cyberdimension.org>
;;;
;;; This file is free software; you can redistribute it and/or modify it
;;; under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3 of the License, or (at
;;; your option) any later version.
;;;
;;; This file is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with GNU Guix.  If not, see <http://www.gnu.org/licenses/>.
;;; The guix.scm file is a convention: A file named guix.scm is
;;; found in several project source code, either in the top
;;; directory or in sub-directories like contrib for instance.

;;; The guix.scm files typically contain package definitions that
;;; are not meant to be used as regular packages but are meant for
;;; testing or developing on the given project.
;;;
;;; For instance here it is used to build libsamsung-ipc in various
;;; configurations. Instead distributions would typically be interested
;;; in only one of the configurations.
;;;
;;; It also copies the sources that are present in the directory that
;;; contains the guix.scm, and builds them. Distributions would instead
;;; be interested in retrieving the source from git or releases tarballs.
;;;
;;; Once you have Guix installed, to build libsamsung-ipc with the
;;; guix.scm file, you can use the 'guix build --file=guix.scm' command,
;;; however as explained above, keep in mind that it will copy all the
;;; files in the same directory than guix.scm, so you might want to
;;; make sure that the sources are clean (with 'make distclean') if you
;;; already built libsamsung-ipc in that directory.
;;;
;;; While this file could also serve as a basis to make a libsamsung-ipc
;;; package in Guix, it is probably a good idea to wait until the API
;;; and ABI changes are complete, ie when the ipc_client has been passed
;;; to all the exported functions, and make a new release when that is
;;; done.

(use-modules
 (ice-9 popen)
 (ice-9 rdelim)
 (sxml ssax input-parse)
 ((guix licenses) #:prefix license:)
 (guix build-system android-ndk)
 (guix build-system gnu)
 (guix gexp)
 (guix git-download)
 (guix packages)
 (gnu packages android)
 (gnu packages autotools)
 (gnu packages disk)
 (gnu packages linux)
 (gnu packages llvm)
 (gnu packages pkg-config)
 (gnu packages python)
 (gnu packages python-xyz)
 (gnu packages tls))

(define %strict-cflags "CFLAGS=-Werror -W -Wall -Wunused -Wunused-function")

(define %commit
  (let* ((port (open-input-pipe
                "git --no-pager log --oneline HEAD -1 --format='%H'"))
         (str  (read-line port)))
    (close-pipe port)
    str))

(define %local-source
  (local-file
   (dirname (current-filename)) #:recursive? #t))

(define-public libsamsung-ipc
  (package
    (name "libsamsung-ipc")
    (version (git-version "0.0" "HEAD" %commit))
    (source %local-source)
    (build-system gnu-build-system)
    (native-inputs
     `(("autoreconf" ,autoconf)
       ("aclocal" ,automake)
       ("ddrescue", ddrescue)
       ("libtool" ,libtool)
       ("pkgconfig" ,pkg-config)
       ("python" ,python)
       ("python-sh" ,python-sh)))
    (inputs
     `(("openssl" ,openssl)))
    (arguments
     `(#:phases
       (modify-phases %standard-phases
         (add-before 'build 'patch-python
           (lambda _
             (substitute* (find-files "tools" ".*\\.py$")
               (("/usr/bin/env python") (which "python3")))
           #t)))))
    (synopsis "libsamsung-ipc is a free software implementation of the Samsung IPC modem protocol")
    (description
     "libsamsung-ipc is a free software implementation of the Samsung IPC modem protocol,
found in many Samsung smartphones and tablets.")
    (home-page "https://www.replicant.us")
    (license license:gpl2+)))

(define-public libsamsung-ipc-gcc-android
  (package
   (inherit libsamsung-ipc)
    (name "libsamsung-ipc-gcc-android")
    (build-system android-ndk-build-system)
    (inputs
     `(("android-libutils" ,android-libutils)
       ("libcrypto" ,openssl)))
    (native-inputs '())
    (arguments
     `(#:phases
       (modify-phases %standard-phases
         (delete 'bootstrap)
         (add-before 'build 'patch-host
           (lambda _
             (substitute* "Android.mk"
               (("BUILD_SHARED_LIBRARY") "BUILD_HOST_SHARED_LIBRARY")
               (("BUILD_STATIC_LIBRARY") "BUILD_HOST_STATIC_LIBRARY")
               (("BUILD_STATIC_EXECUTABLE") "BUILD_HOST_STATIC_EXECUTABLE"))
             #t)))
       #:make-flags (list ,%strict-cflags)))))

(define-public libsamsung-ipc-gcc-autotools
  (package
   (inherit libsamsung-ipc)
   (name "libsamsung-ipc-gcc-autotools")
   (arguments
    `(#:phases
      (modify-phases %standard-phases
                     (add-before 'build 'patch-python
                                 (lambda _
                                   (substitute* (find-files "tools" ".*\\.py$")
                                                (("/usr/bin/env python") (which "python3")))
                                   #t)))
      #:make-flags (list ,%strict-cflags)))))

(define-public libsamsung-ipc-clang-autotools
  (package
   (inherit libsamsung-ipc)
   (name "libsamsung-ipc-gcc-autotools")
    (native-inputs
     `(("autoreconf" ,autoconf)
       ("aclocal" ,automake)
       ("ddrescue", ddrescue)
       ("libtool" ,libtool)
       ("pkgconfig" ,pkg-config)
       ("python" ,python)
       ("python-sh" ,python-sh)
       ("clang" ,clang-10)))
    (arguments
     `(#:phases
       (modify-phases %standard-phases
         (add-before 'build 'patch-python
           (lambda _
             (substitute* (find-files "tools" ".*\\.py$")
               (("/usr/bin/env python") (which "python3")))
             #t)))
     #:make-flags (list ,%strict-cflags)))))

(define-public libsamsung-ipc-clang-android
  (package
   (inherit libsamsung-ipc)
    (name "libsamsung-ipc-clang-android")
    (build-system android-ndk-build-system)
    (inputs
     `(("android-libutils" ,android-libutils)
       ("libcrypto" ,openssl)
       ("clang" ,clang-10)))
    (native-inputs '())
    (arguments
     `(#:phases
       (modify-phases %standard-phases
         (delete 'bootstrap)
         (add-before 'build 'patch-host
           (lambda _
             (substitute* "Android.mk"
               (("BUILD_SHARED_LIBRARY") "BUILD_HOST_SHARED_LIBRARY")
               (("BUILD_STATIC_LIBRARY") "BUILD_HOST_STATIC_LIBRARY")
               (("BUILD_STATIC_EXECUTABLE") "BUILD_HOST_STATIC_EXECUTABLE"))
             #t))
         (add-after 'patch-host 'prepare-build-environment
                    (lambda* (#:key inputs #:allow-other-keys)
                      (setenv "CC" "clang")
                      #t)))
       #:make-flags (list ,%strict-cflags)))))

;; Combinaisons:
;; +--------------------------------+----------+----------+--------------+--------------+
;; | Package name                   | Compiler | Compiler | Build system | Comments     |
;; |                                |          | flags    |              |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc                 | GCC      | none     | autotools    | Base package |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-gcc-android     | GCC      | strict   | Android.mk   |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-gcc-autotools   | GCC      | strict   | autotools    |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-clang-android   | clang    | strict   | Android.mk   |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-clang-autotools | clang    | strict   | autotools    |              |
;; +--------------------------------+----------+----------+--------------+--------------+

(list libsamsung-ipc
      libsamsung-ipc-gcc-android
      libsamsung-ipc-gcc-autotools
      libsamsung-ipc-clang-android
      libsamsung-ipc-clang-autotools)
