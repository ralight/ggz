SUBDIRS = src pics doc variants

EXTRA_DIST = module.dsc.in

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.ds

