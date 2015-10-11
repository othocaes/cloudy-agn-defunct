#!/usr/local/bin/bash

script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')
runs_dir=$script_dir/../thor_runs

date > $runs_dir/refresh_date

rsync -avv xaw5719@thor.cs.wmich.edu:/home/research/xaw5719/cloudy/runs/good/ $runs_dir


