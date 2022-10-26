# -*- mode: ruby -*-
# vi: set ft=ruby :

# if you update this file, please consider updating .travis.yml too

require 'yaml'

current_dir    = File.dirname(File.expand_path(__FILE__))
configfile        = YAML.load_file("#{current_dir}/.vagrantconfig.yml")
yaml_config = configfile['configs']['dev']

Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/focal64"
  config.vm.provider :virtualbox do |vb|
    vb.customize ["modifyvm", :id, "--memory", "4096"]
    vb.customize ["modifyvm", :id, "--cpus", "1"]
  end

  $config_shell = <<-'SHELL'
     set -e
     set -x

     sudo apt-get update
     sudo apt-get dist-upgrade -y
     sudo apt-get install -y ccache wget git build-essential

     sudo apt-get install -y libglu1-mesa-dev

     sudo apt-get install -y aptitude
     sudo aptitude install -y flite1-dev libsdl2-dev libsdl1.2-dev libsndfile-dev libssl-dev libudev-dev

     sudo aptitude install -y \
          python3-serial \
          python3-pexpect

     # taken from travis.yml
     echo 'Initialising submodules'
     su - vagrant -c 'cd %{project_root_dir}; git submodule init && git submodule update'

     # with reference to https://github.com/jurplel/install-qt-action/blob/master/src/main.ts and .github/workflows/linux_release.yml:
     echo 'Installing QT'
     apt-get install -y python3-pip
     su - vagrant -c "pip3 install --user aqtinstall"

     apt-get install -y patchelf

     dir="%{qt_deps_unpack_dir}"
     version="%{qt_deps_ver}"
     host="linux"
     target="desktop"
     modules="qtcharts"
     su - vagrant -c "rm -rf ${dir}"
     su - vagrant -c "mkdir -p ${dir}"
     su - vagrant -c "python3 -m aqt install-qt -O ${dir} ${host} ${target} ${version} -m ${modules}"


     # copy Qt deps into the shadow-build directory so that the
     # compiled binary can find them in its LD_LIBRARY_PATH:
     su - vagrant -c 'mkdir -p %{qt_deps_dir}'
     su - vagrant -c 'cp -a %{qt_deps_bin_unpack_dir} %{qt_deps_bin_dir}'
     su - vagrant -c 'cp -a %{qt_deps_lib_unpack_dir} %{qt_deps_lib_dir}'
     su - vagrant -c 'cp -a %{qt_deps_plugins_unpack_dir} %{qt_deps_plugins_dir}'
     su - vagrant -c 'cp -a %{qt_deps_qml_unpack_dir} %{qt_deps_qml_dir}'

     # write out a pair of scripts to make rebuilding on the VM easy:
     su - vagrant -c "cat <<QMAKE >do-qmake.sh
#!/bin/bash

set -e
set -x

cd %{shadow_build_dir}
export LD_LIBRARY_PATH=%{qt_deps_lib_unpack_dir}
export PATH=%{qt_deps_bin_unpack_dir}:\$PATH
qmake -r %{pro} CONFIG+=\${CONFIG} CONFIG+=WarningsAsErrorsOn -spec %{spec}
QMAKE
"

     su - vagrant -c "cat <<MAKE >do-make.sh
#!/bin/bash

set -e
set -x

cd %{shadow_build_dir}
export LD_LIBRARY_PATH=%{qt_deps_lib_unpack_dir}
export PATH=%{qt_deps_bin_unpack_dir}:\$PATH
make -j1
MAKE
"
    su - vagrant -c "chmod +x do-qmake.sh do-make.sh"

    # now run the scripts:
    su - vagrant -c ./do-qmake.sh
    su - vagrant -c ./do-make.sh

   SHELL

  config.vm.provision "dev", type: "shell", inline: $config_shell  % {
    :shadow_build_dir => yaml_config['shadow_build_dir'],
    :pro => yaml_config['pro'],
    :spec => yaml_config['spec'],
    :project_root_dir => yaml_config['project_root_dir'],

    :qt_deps_ver => yaml_config['qt_deps_ver'],

    :qt_deps_unpack_parent_dir => yaml_config['qt_deps_unpack_parent_dir'],
    :qt_deps_unpack_dir => yaml_config['qt_deps_unpack_dir'],
    :qt_deps_bin_unpack_dir => yaml_config['qt_deps_bin_unpack_dir'],
    :qt_deps_lib_unpack_dir => yaml_config['qt_deps_lib_unpack_dir'],
    :qt_deps_plugins_unpack_dir => yaml_config['qt_deps_plugins_unpack_dir'],
    :qt_deps_qml_unpack_dir => yaml_config['qt_deps_qml_unpack_dir'],

    :qt_deps_dir => yaml_config['qt_deps_dir'],
    :qt_deps_bin_dir => yaml_config['qt_deps_bin_dir'],
    :qt_deps_lib_dir => yaml_config['qt_deps_lib_dir'],
    :qt_deps_plugins_dir => yaml_config['qt_deps_plugins_dir'],
    :qt_deps_qml_dir => yaml_config['qt_deps_qml_dir'],
  }


end
