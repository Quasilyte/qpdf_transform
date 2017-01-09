#!/bin/bash

#####
# (!) Запускать из корня репозитория qpdf_transform
#####

# Предварительная подготовка
echo ' -- making preparations'
rm -rf rpm/*
mkdir -p rpm/{RPMS,SRPMS,SPECS,SOURCES,BUILD}
mkdir -p tmp

# Конфигурируемые параметры
pkg_version='1.0'
pkg_name='qpdf_transform'
tmp_dir=`pwd`/tmp # Либо "/var/tmp"

# Подготавливаем архив для SOURCES
echo ' -- preparing SOURCES tar.gz'
tar -czvf rpm/SOURCES/$pkg_name-$pkg_version.tar.gz src/* Makefile

# Генерация spec-файла
echo ' -- generating SPECS file'
cat <<EOF > rpm/SPECS/qpdf_transform.spec
#########################################
Name: $pkg_name
Version: $pkg_version
Release: 1
URL: https://github.com/Quasilyte/qpdf_transform
Summary: Program to transform input PDF. Can reorder pages, rotate them, omit pages in single operation
Group: Applications/file
License: MIT
Source: %{name}-%{version}.tar.gz

# BuildRequires:
# Requires:

%description
Maps input PDF into output PDF using [page, rotation] lists.
It can be used to create a PDF copy which contains some pages removed,
other pages rotated and some pages reordered.

%prep
%setup -q -n %{name}-%{version}

%build
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/local/{bin,lib,share}
install qpdf_transform $RPM_BUILD_ROOT/usr/local/bin

%files
%{_bindir}/qpdf_transform

%changelog
#########################################
EOF

echo ' -- done'
