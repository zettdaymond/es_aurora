%define __provides_exclude_from ^%{_datadir}/.*$
%define __requires_exclude ^librenpy.*$

Name:       su.sovietgames.everlasting_summer
Summary:    Порт визуальной новеллы "Бесконечное Лето" от SovietGames для Авроры
Version:    1.6.1
Release:    1
License:    CC BY-NC-SA
URL:        https://everlastingsummer.su
Source0:    %{name}-%{version}.tar.bz2

Requires: libdbus-1.so.3
Requires: libglib-2.0.so.0
Requires: libaudioresource.so.1
Requires: libwayland-client.so.0

%description
Порт визуальной новеллы "Бесконечное Лето" от SovietGames для ОС Аврора

Официальный сайт игры: https://everlastingsummer.su/ru/

Встретив на улице Семена, главного героя игры, вы бы никогда не обратили на него внимания - действительно, подобных людей в каждом городе тысячи и даже сотни тысяч.
Но однажды с ним приключается совершенно необычное происшествие: он засыпает зимой в автобусе, а просыпается... посреди жаркого лета.
Перед ним – пионерлагерь «Совенок», а позади – прошлая жизнь.
Чтобы разгадать, что же с ним случилось, Семену придется получше узнать местных обитателей (а может, даже встретить любовь),
разобраться в лабиринтах сложных человеческих взаимоотношений и своих собственных проблемах, решить загадки лагеря.
И главное – как вернуться обратно или не возвращаться вовсе?

Управление – проведите по экрану пальцем:

• Вверх, чтобы открыть меню.
• Вправо, чтобы включить перемотку.
• Влево, чтобы открыть историю сообщений.
• Вниз, чтобы скрыть интерфейс.

По проблемам, багам связанными с портом можно писать на почту zettday@gmail.com

%prep
%autosetup

%build
%cmake -GNinja
%ninja_build

%install
%ninja_install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/%{name}

%{_datadir}/%{name}/main.py
%dir %{_datadir}/%{name}/lib/
%dir %{_datadir}/%{name}/renpy/

%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
