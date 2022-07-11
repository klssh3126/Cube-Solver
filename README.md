# Cube-Solver <br/>

### 캡스톤디자인 큐브솔버 https://www.youtube.com/watch?v=H08CGIq9q4M   <br/>
<br/>

![Cube_solver_version2_cropped](https://user-images.githubusercontent.com/35258559/178189067-6db1f332-aa54-4000-b453-476c7bd77eaa.gif)
<br/><br/>
       
![Cube_solver_version2](https://user-images.githubusercontent.com/35258559/178189086-e5ea1ef4-760f-4bd4-9ce8-4bab308f0f68.gif)
<br/> <br/>
       
![Cube_solver_version2 (1)](https://user-images.githubusercontent.com/35258559/178189089-ad959116-f2cd-4c8f-ac76-c647ddd18ecf.gif)
<br/><br/><br/><br/>

# 목차 
[1. 사용기술](#사용-기술) <br/>
[2. 프로젝트 수행 회고](#프로젝트-수행-회고) <br/>
[3. 큐브 풀이법 계산](#큐브-풀이법-계산) <br/>
[4. 영상처리를 통한 색깔 판별](#영상처리를-통한-색깔-판별) <br/>
[5. 기계 CAD 디자인 및 조립](#기계-CAD-디자인-및-조립) <br/>
[6. 모터제어](#모터제어) <br/>
[7. 프로젝트 결과](#프로젝트-결과) <br/><br/> <br/>


## 1. 사용 기술
### C++, openCV, RaspberryPi3, Inventor, 3D 프린터
<br/><br/><br/>

## 2.  프로젝트 수행 회고 
나는 C++ 과 openCV의 책을 사서 혼자 예제를 따라해본 경험이 있었다.  그러나 이러한 프로젝트를 수행해본 적은 없었다. 프로젝트는 연구실의 석사분이 큐브 장난감을 들고 왔던것 부터 시작이었다. 나는 큐브를 맞출 줄 모르지만, 이러한 큐브를 자동으로 맞추는 기계가 있다면 좋겠다는 생각이 들었고, 궁금해서 유튜브에 찾아보니 실제로 로봇이 있었다. 여기에 사용된 기술들을 분석해보니 C++, openCV 를 할 줄 알면 만들 수 있다는 생각이 들었다. <br/>

![슬라이드1](https://user-images.githubusercontent.com/35258559/178184463-30b085d3-b86c-40cc-9064-63f7018703db.JPG)   <br/>
![슬라이드2](https://user-images.githubusercontent.com/35258559/178184465-702cdec0-be0e-432c-9c3f-21f96dce146f.JPG)   <br/><br/><br/>

## 3. 큐브 풀이법 계산
처음에는 큐브를 풀어주는 해법부터 먼저 생각을 했다. 아이디어에 대해서 검색을 하고 연구실 선배들과 이야기를 하다가, 배열을 활용해서 전개도를 만들어 보라는 조언을 얻었고, 거기서 부터 프로그래밍을 시작을 하였다. 배열을 활용하여 가상의 전개도를 상상하였고, 회전을 하면 그 전개도에 있는 어떤 값이 다른 전개도의 칸에 옮겨지는 방식으로, 회전을 의미하는 함수를 구현을 하였다.<br/>
![슬라이드6](https://user-images.githubusercontent.com/35258559/178184473-90c119a0-5e0d-4c03-aca1-237fb3111e56.JPG)   <br/><br/><br/>

그 다음은 구글을 검색하여 장난감 큐브를 푸는 방법을 검색하였다. 나는 장난감 큐브를 풀 줄 모른다. 다만 설명서를 보고 따라 해보며 풀어본적은 있다. 그 설명서를 구하여 장난감 큐브를 푸는 방법을 구현을 하였다. 큐브를 맞추는 과정은 7단계로 구성이 되는데 처음에는 큐브의 한 면을 십자가를 맞춘다음, 한 면을 완성하고, 그 옆면 4면을 완성후, 마지막 남은 반대편 면을 완성하는 단계로 순차적으로 되어있다. <br/>
![큐브풀이순서](https://user-images.githubusercontent.com/35258559/178190638-5c56f8f1-1124-4d98-80ba-ce1ec6fd6986.JPG)   <br/><br/><br/>

설명서에는 각 단계별로, 우리가 맞추고자 하는 색깔이 어떻게 배치되어 있느냐에 따라 회전을 어떻게 하면 되는지 표시가 되어 있었다. 나는 이 부분을 코드로 구현하였다. 즉, 전개도를 보며, 우리가 원하는 색깔(배열값) 이 전개도에 어디에 있다면, 그 경우에 맞게 풀이법대로 회전 함수를 순서에 맞게 호출해주는 것이다. 회전 함수는 호출될 때마다 값을 리턴하며, 큐브의 회전순서를 의미하는 배열에 계속 저장이 된다. 이렇게 7단계를 전부 거치고 나면, 결국에는 큐브를 완성해주는 순서가 담겨있는 배열이 완성된다. 또한 이러한 순서 배열은 최적화 된 값이 아니다. <br/>
![슬라이드7](https://user-images.githubusercontent.com/35258559/178184475-12f4ef98-0d63-4897-bc49-d7171e918d5e.JPG)   <br/>
![슬라이드8](https://user-images.githubusercontent.com/35258559/178184478-1a12b255-eae6-4dc3-b615-277cb4a459fc.JPG)   <br/>
![슬라이드10](https://user-images.githubusercontent.com/35258559/178184479-e0af3f31-fc98-4fa4-9505-ef1152203546.JPG)   <br/><br/><br/>

즉 결과를 살펴보면 B-> B -> B -> B (여기서 B는 뒷면을 시계방향으로 돌리는 것을 의미) 이렇게 같은 방향의 회전이 4번 반복되는 경우가 있는데, 이것은 사실 한번도 돌리지 않은 것과 같다. 또한 B->B->B 처럼 B가 같은 방향의 회전이 3번 반복될 경우에는 Bi로 치환해준다. 시계방향으로 큐브를 3번 돌린것은 반시계 방향으로 한번 돌린것과 동일하기 때문이다. 이러한 코드를 적용한 덕분에 135번의 회전을 124번의 회전으로 줄일 수 있었다.<br/>
![슬라이드11](https://user-images.githubusercontent.com/35258559/178184480-0626e839-c653-47e6-9781-5e788fb64319.JPG)   <br/><br/><br/>

중간중간에, 전개도에 내가 색깔값을 넣어보며 실제로 해법을 잘 계산해 주는지, 확인을 해보았다. 무한루프에 빠지면 어디가 문제인지 파악을 하였다. 그 원인은 주로 해법의 경우의 수를 덜 구현을 하였거나, 함수에서 전개도 값을 옮길 때 인덱스를 잘못 타이핑 하여 값이 엉뚱한 배열에 들어간 경우가 많았다. 이 과정을 해결하는데 시간이 오래 걸렸었다. 결국은 시행착오 끝에 완전히 구현을 할 수 있었다.<br/>

- - - 
## 4. 영상처리를 통한 색깔 판별
다음은 웹캠과 openCV 라이브러리를 활용하여 반복문을 활용하여 이미지를 계속 불러왔다. 특히 색깔을 판별하는 알고리즘이 어려웠다. 구글링을 계속하며 아이디어를 얻게 되었는데, 유클리드 거리를 구하는 방법을 이용하면 된다. 즉, 카메라에서 화소의 데이터를 읽어보면 R G B 각각 한바이트씩 0~255 값을 갖게된다. 이 떄, 색깔 별로, 화소값을 10번 정도 얻어와서 평균을 내었고, 그 평균값을 그 색깔의 대표 화소값이라 정하였다. 그 후, 큐브를 촬영할 때, 새로 얻어진 화소값과, 내가 이미 측정한 색깔별 평균 화소값과 차이의 제곱의 합이 가장 최소가 되는 색깔이 바로, 내가 촬영한 큐브 면의 색깔로 판별이 되는 것이다. 이것을 코딩으로 구현하였다. 예전에 캡스톤디자인 심사 담당 교수님께서 큐브를 어떻게 인지하느냐고 물어보았는데, 나는 큐브를 인지할 필요가 없다고 말씀드렸다. 왜냐하면, 기계팔(그리퍼)가 큐브를 거의 똑같은 지점에서 물고있기 때문에, 웹캠이 고정되어 촬영하는 위치에 큐브를 계속 잡아낸다. 그러니, 이미지 상에서 큐브가 거의 항상 같은 자리에 위치하고 있기 때문에 화소값의 위치를 항상 고정된 곳에서 읽어와도 큰 문제가 없었다. <br/>
![슬라이드5](https://user-images.githubusercontent.com/35258559/178184472-82b7534c-9e2f-4573-993f-0dd792401a59.JPG)   <br/>
![슬라이드4](https://user-images.githubusercontent.com/35258559/178184470-e05cf2e7-f752-40c2-8d83-cce5b9de72ae.JPG)   <br/>
![슬라이드3](https://user-images.githubusercontent.com/35258559/178184468-209b4029-3836-41fa-8445-fed53209d55e.JPG) <br/>  <br/> 
 <br/> 


- - -
## 5. 기계 CAD 디자인 및 조립
그 다음은 기계부분이다. 1학년 때 카티아로 3D 모델링을 해본 경험이 있었기 때문에 3D CAD툴에 대한 거부감은 없었다. 그냥 A4용지에 연필로 그림을 그려가며 치수를 예상하였고, 3D 프린터로 계속 출력해보며 부품들을 뽑아서 조립하였다. 톱니바퀴 설계랑 톱니바퀴랑 맞물리는 랙을 설계하는게 까다로웠다. 또한 치수가 조금이라도 안맞으면 기계가 조립이 안되는 경우도 많았고, 3D프린터도 계속 노즐이 막히거나 모터가 고장나서 내가 직접 고쳐가며 출력해나갔다. 출력을 해도 조립할 때 부품간에 공차가 안맞으면 조립이 불가능하여 다시 도면을 수정하고 다시 출력하는 과정을 많이 거쳤다. 시간이 상당히 오래걸리는 작업이었다.  <br/>

![슬라이드13](https://user-images.githubusercontent.com/35258559/178184482-d3625510-cdc0-4020-b9e3-502f661352fd.JPG) <br/>
![슬라이드14](https://user-images.githubusercontent.com/35258559/178184483-bcf7a71e-09d3-494b-b227-061c05261254.JPG)   <br/>
![슬라이드15](https://user-images.githubusercontent.com/35258559/178184484-95e290e1-d908-44b5-b0fd-b397dadde059.JPG)   <br/> <br/> <br/>


![슬라이드16](https://user-images.githubusercontent.com/35258559/178184487-d3c15113-0d0e-47fc-aa7e-fa62e55acfa0.JPG)   <br/>
![슬라이드17](https://user-images.githubusercontent.com/35258559/178184488-dc6b2ce9-37a7-4a98-a8a3-6d21b03b7814.JPG)   <br/><br/> <br/>

## 6. 모터제어
마지막은 라즈베리파이에 서보모터 제어 라이브러리를 설치하여 서보모터 5개와 스텝모터2개를 연결하여 브레드보드에 전원을 넣어주었다. 또한 각각 기계팔과 서보모터를 회전시켜 주었다. 두번째 문단에서 큐브를 완성해 주는 순서가 담겨있는 배열을 반복문으로 읽어보며, 배열에 들어있는 각각의 값( 앞면회전, 윗면회전, 좌측면 반시계방향 회전 등) 에 따라 동작할 일련의 모터 제어 순서를 코드로 정해 두었다. 즉 앞면을 시계방항으로 회전시킨다고 하면, 각각 기계팔이 어떻게 움직여야 할지 일련의 동작 순서들을 실행할 함수들을 만들었다.  <br/>
![슬라이드12](https://user-images.githubusercontent.com/35258559/178184481-6b57f9a2-22ff-4460-b8ad-bd70f1f80b15.JPG)  <br/>
![회로도](https://user-images.githubusercontent.com/35258559/178190585-e35f6089-4728-4392-82f7-0862b42ec081.JPG)<br/>
![슬라이드21](https://user-images.githubusercontent.com/35258559/178184493-82f6b0e6-c0ff-4fff-a29b-59c07ea4c29e.JPG)   <br/><br/>
![슬라이드22](https://user-images.githubusercontent.com/35258559/178184494-d51e75bb-c58c-42a7-a935-0eaa9056d280.JPG)   <br/><br/>
![슬라이드23](https://user-images.githubusercontent.com/35258559/178184496-faa84bdb-9ea3-4cea-94ab-093c07ddc9ea.JPG)   <br/>
![181119 - 캡디 마지막 발표자료](https://user-images.githubusercontent.com/35258559/178198152-6bb8f59e-5c41-45cf-9533-5e95d3358092.jpg) <br/>
![181119 - 캡디 마지막 발표자료](https://user-images.githubusercontent.com/35258559/178198998-70985690-ea66-4ae0-b395-9062662a84cf.jpg) <br/><br/> <br/>


## 7. 프로젝트 결과
이렇게 해서 큐브를 촬영해주고 스스로 풀이법을 계산하여 기계팔을 돌려 완성시켜주는 로봇을 완성하였고, 캡스톤디자인1, 캡스톤디자인2 모두 전기공학부 최우수상을 수상할 수 있었다.








