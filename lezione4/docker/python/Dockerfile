# docker build -t pymqtt:0.1 .

FROM python:3.10.11-bullseye

RUN useradd -ms /bin/bash user
WORKDIR /home/user

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

USER user
