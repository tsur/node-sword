# node:8.0.0 is a DEBIAN based image
FROM node:8.0.0
MAINTAINER Zuri Pabon <zurisadai.pabon@gmail.com>
RUN mkdir -p /usr/src/node_sword
WORKDIR /usr/src/node_sword
COPY . /usr/src/node_sword
RUN npm start
CMD ["npm", "run", "build"]
