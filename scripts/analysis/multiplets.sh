cloudy_output_file=$1
emergent_lines_start=`grep -n "^\s*Emergent line intensities\s*$" $cloudy_output_file|tail -n1|sed 's@\([0-9]*\):.*@\1@'`
total_lines=`wc -l $cloudy_output_file|sed 's@^\([0-9]*\)[^0-9]*.*$@\1@'`
emission_line_list=`sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file`

echo "Requested Emergent Emission Line Fluxes"
echo "======================================"
echo "Unit Intensity"
echo "--------------"
sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep 1.0000
echo " "

# Possible Multiplets - Rough
echo -n "Possible Multiplets - Rough"
TOTL_lines=`grep -n TOTL $cloudy_output_file|sed 's#\([0-9]\): TOTL.*#\1#'`
list_size=`echo $TOTL_lines|wc -w`
echo " (found ${list_size})"
echo "-------------"
for line_number in $TOTL_lines; do
	sed -n $line_number,$((line_number+14))p $cloudy_output_file
	echo "==================================================="
done
echo " "