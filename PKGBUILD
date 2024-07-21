# Maintainer: Henry Wandover <henrywandover@gmail.com>
pkgname=hrry-shell
pkgver=r72.08dab27
pkgrel=1
pkgdesc="An emoji interactive shell and scripting language."
arch=("x86_64")
url="https://github.com/yung-turabian/hrry-shell"
license=("custom")
#groups=()
#provides=()
#replaces=()
#backup=()
#options=()
install=hrry.install
#changelog=
#source=($pkgname-$pkgver.tar.gz)
source=("git+${url}#branch=trunk")
noextract=()
md5sums=('SKIP') #autofill using updpkgsums

#build() {
#  cd "$pkgname-$pkgver"

#  ./configure --prefix=/usr
#  make
#}

pkgver() {
		cd "${srcdir}/${pkgname}"
		printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
		cd "${srcdir}/hrry-shell"
		git pull origin trunk 

		./build_tools/build_with_ninja.sh
}

package() {
		
		mkdir -p "${pkgdir}/usr/bin"
		mkdir -p "${pkgdir}/usr/share"
		mkdir -p "${pkgdir}/usr/share/doc"
		mkdir -p "${pkgdir}/usr/share/hrry"
		mkdir -p "${pkgdir}/usr/share/hrry/completions"
		mkdir -p "${pkgdir}/usr/share/info"
		mkdir -p "${pkgdir}/usr/share/licenses"
		mkdir -p "${pkgdir}/usr/share/licenses/hrry"
		mkdir -p "${pkgdir}/usr/share/man"
		mkdir -p "${pkgdir}/usr/share/man/man1"

		#cd "$pkgname-$pkgver"

		#make DESTDIR="$pkgdir/" install

		cp "${srcdir}/hrry-shell/hrry" "${pkgdir}/usr/bin/hrry"
		
		chmod +x "${pkgdir}/usr/bin/hrry"

}
