include $(TOPDIR)/rules.mk

PKG_NAME:=tuyad
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/tuyad
	CATEGORY:=Base system
	TITLE:=tuyad
	DEPENDS:=+tuya-iot-core-sdk +libubox +libubus +argp-standalone +liblua
endef

define Package/tuyad/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/lib/tuya/modules

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuyad $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/*.h   $(1)/usr/include/
	$(INSTALL_BIN) ./files/tuyad.init     $(1)/etc/init.d/tuyad
	$(INSTALL_CONF) ./files/tuyad.config  $(1)/etc/config/tuyad
	$(INSTALL_BIN) ./modules/*.lua        $(1)/usr/lib/tuya/modules/
endef

$(eval $(call BuildPackage,tuyad))