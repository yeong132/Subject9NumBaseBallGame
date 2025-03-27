# 📌개요
- 이 프로젝트는 숫자 야구 게임입니다.<br>
- 원조 게임은 4자리 숫자로 하는 Bulls and Cows로 이 게임은 3자리 숫자로 진행됩니다.<br>
- 총 플레이어는 2명이고, 각자 목표 숫자를 맞출 기회를 3번씩 가집니다.<br>
- 멀티플레이 게임이며, 정답을 맞출 기회가 소진될 동안 목표 숫자를 더 빨리 맞추는 플레이어가 이기게 됩니다.<br>
- 게임이 끝나면 결과를 보여주고 5초 뒤에 새 게임이 시작됩니다.

<strong>하지만, 정확히 3자리가 숫자가 아닌 틀린 입력을 하게 되면 바로 상대 플레이어가 이기게 되니 주의하십시오.</strong>

# 📌기능
## 💎 기본 기능
### GameMode 서버 로직
채팅 기반으로 명령어를 입력하면 서버가 이를 해석해서 게임 로직을 처리합니다.<br>
게임 판정이나 흐름은 서버쪽인 게임모드가 주로 관리하도록 하였습니다.

---
### 3자리 난수 생성 로직 (중복 없는 1~9 숫자)
게임 시작시 서버에서 무작위 3자리 숫자를 만듭니다. 각 숫자마다 중복은 불가능합니다.<br>
예를 들어, 123, 456, 789는 가능하지만 111, 112, 999 같은 숫자는 만들어지지 않습니다.

---
### 판정 로직 (S/B/OUT 계산)
서버가 생성한 숫자와 플레이어가 입력한 숫자를 비교해서 스트라이크(S), 볼(B), 아웃(OUT)을 계산합니다.<br>
예를 들어, 서버 숫자가 123이고 플레이어 숫자가 132면 두 숫자의 자리수를 하나씩 비교해가는 것이다.<br>

첫번째 자리수가 같으니 판정 메시지는 "1S"<br>
두번째 자리수는 다르지만 값이 존재하니까 판정 메시지는 "1S 1B"<br>
세번째 자리수도 다르지만 값이 존재하니까 판정 메시지는 "1S 2B"

---
### 시도 횟수 및 상태 관리
중복 숫자가 포함된 입력, 3자리가 아닌 입력은 아웃(OUT) 처리됩니다.<br>
입력할 때마다 플레이어별 남은 시도 횟수를 갱신합니다.

---
### 승리, 무승부, 게임 리셋
#### 승리 조건
목표 숫자를 더 빨리 맞추는 플레이어가 승리하게 됩니다.<br>
상대 플레이어가 잘못된 입력을 하면 즉시 승리하게 됩니다.
#### 무승부 조건
둘 다 목표 숫자를 맞추지 못하고 기회를 모두 소진하면 무승부하게 됩니다.
#### 게임 리셋 조건
승리하거나 무승부하면 게임이 리셋되어야 합니다.<br>
결과는 보고 재시작해야하니 게임이 끝난 후 5초 뒤에 재시작됩니다.

---
## 💎 고려사항
좀 더 신경써서 만든 부분을 기록했습니다.
### C++ 라이브러리 분리
판정(S/B 계산)과 난수 생성 로직은 게임 흐름에 영향없이 그저 계산만 해주는 로직이기 때문에<br>
코드 가독성을 위해 서버에서 처리되는 순수 계산만 하는 로직을 C++ 라이브러리로 분리했습니다.<br>
GameMode에서 판정과 난수 생성을 하더라도 그 계산 로직은 다른 라이브러리의 함수에서 결과를 받아와 사용했습니다.

### 클린코드 만들기
아직은 뭐가 제대로 된 클린코드인지 모르는 단계지만 최대한 배운것들을 써서 코드를 리팩토링했습니다.


---
