clf;

hold on;
for limit = 4000:2000:16000
	filename = sprintf("fixed-encoding-time-%05d.dat", limit);
	exps = importdata(filename, "\t")
	plot(exps.data(:,2)/10^6, exps.data(:,1), "-o");
endfor
hold off;

xlim([0 5])
ylim([70 100])

title("Pareto front with fixed encoding time")
xlabel("Bitstream size (Mo)")
ylabel("VMAF score")
legend(
 "4000 ms encoding time",
%  "5000 ms encoding time",
 "6000 ms encoding time",
%  "7000 ms encoding time",
 "8000 ms encoding time",
%  "9000 ms encoding time",
"10000 ms encoding time",
% "11000 ms encoding time",
"12000 ms encoding time",
% "13000 ms encoding time",
"14000 ms encoding time",
% "15000 ms encoding time",
"16000 ms encoding time",
"location", "southeast"
)

saveas(1, "pareto-fixed-encoding-time.png");
