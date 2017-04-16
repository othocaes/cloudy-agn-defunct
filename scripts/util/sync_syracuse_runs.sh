#!/usr/bin/env bash

script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')
runs_dir=$script_dir/../syracuse_runs

date > $runs_dir/refresh_date

rsync -avv --exclude 'reports' --exclude 'mnt' --exclude 'inc' --exclude 'devel' --exclude 'papers' --exclude 'proc' caes@192.168.1.11:/mnt/science/agn/cloudy/runs/ $runs_dir


