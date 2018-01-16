FROM andrewosh/binder-base

MAINTAINER Kozo Nishida <knishida@riken.jp>

USER root

RUN apt-get update
RUN apt-get install -y build-essential ruby ruby-dev rake git libzmq3 libzmq3-dev libgsl0-dev libtool autoconf automake zlib1g-dev && apt-get clean
RUN ln -s /usr/bin/libtoolize /usr/bin/libtool # See https://github.com/zeromq/libzmq/issues/1385
RUN git clone git://github.com/ruby-numo/numo-narray
RUN gem update --no-document --system && gem install --no-document iruby pry rbczmq
RUN cd narray; gem build numo-narray.gemspec; gem install numo-narray-*.gem

USER main

RUN iruby register
