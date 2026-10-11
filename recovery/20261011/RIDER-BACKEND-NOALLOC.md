# Rider hot-packet allocation correction

Apply after frozen backend package. selected_packet.cpp preimage is the exact
frozen backend candidate; model_packet.cpp preimage is active source ccaca82.
Replaces temporary std::string-producing archive.find calls inside noexcept hot
functions with allocation-free exact canonical catalog identity lookup. Existing
authenticated archive admission and member pointer checks are retained. Actual
catalog spellings are UI\UIRider\{Widgets.Dat,rider.dat,ui.dat}.

No builds, syntax checks, project parser execution or tests were run.
