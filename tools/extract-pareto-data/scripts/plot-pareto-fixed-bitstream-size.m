clf;

hold on;
mega = 1024 * 1024;
for limit = 0.7*mega:0.2*mega:1.9*mega
	filename = sprintf("fixed-bitstream-size-%07d.dat", round(limit));
	exps = importdata(filename, "\t");
	plot(exps.data(:,3)/10^3, exps.data(:,1), "-o");
endfor
hold off;

xlim([0 15])
ylim([70 100])

title("Pareto front with fixed encoding time")
xlabel("Encoding time (s)")
ylabel("VMAF score")
legend(
 "0.7 Mo bitstream size",
%  "0.8 Mo bitstream size",
 "0.9 Mo bitstream size",
%  "1.0 Mo bitstream size",
 "1.1 Mo bitstream size",
%  "1.2 Mo bitstream size",
 "1.3 Mo bitstream size",
%  "1.4 Mo bitstream size",
 "1.5 Mo bitstream size",
%  "1.6 Mo bitstream size",
 "1.7 Mo bitstream size",
%  "1.8 Mo bitstream size",
 "1.9 Mo bitstream size",
"location", "southeast"
)

saveas(1, "pareto-fixed-bitstream-size.png");
